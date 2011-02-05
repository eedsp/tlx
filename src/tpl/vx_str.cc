//
// Created by Shiwon Cho on 2005.10.27.
//

#include <iostream>
#include "vx_str.h"

#define UPDATE_UCS_offset(p_offset_utf8, p_offset_utf8_s, pSTR, pSTR_len, p_offset_ucs) { \
    int32_t __v_idx_utf8 = p_offset_utf8; \
    while (__v_idx_utf8 < p_offset_utf8_s && __v_idx_utf8 < pSTR_len) \
    { \
        UChar32 __c = 0; \
        U8_NEXT (pSTR, __v_idx_utf8, pSTR_len, __c); \
        p_offset_ucs++; \
    } \
}

#define UPDATE_UCS_len(pSTR, pSTR_len, p_len_ucs) { \
    int32_t __v_idx_utf8 = 0; \
    while (__v_idx_utf8 < pSTR_len) \
    { \
        UChar32 __c = 0; \
        U8_NEXT (pSTR, __v_idx_utf8, pSTR_len, __c); \
        p_len_ucs++; \
    } \
}

static const char *_TPL_DEFAULT_TAG = "_PHRASE_";

vx_str::vx_str (apr_pool_t *p_pool, const hx_shm_rec_t *p_shm_t)
{
    v_pool = nullptr;

    vSTR = nullptr;
    vSTR_len = 0;

    v_number_of_codes = 0;
    v_list_of_codes = nullptr;
    v_ctx_general = nullptr;

    v_error_code = v_error_offset = 0;

    v_name_count = nullptr;
    v_name_entry_size = nullptr;
    v_name_table = nullptr;
    v_name_entry_max_size = 0;

    v_match_context = nullptr;
    v_jit_stack = nullptr;
    v_match_data = nullptr;
    v_out_vector = nullptr;

    v_token_list = nullptr;

    v_status = APR_SUCCESS;
    v_status = apr_pool_create (&v_pool, p_pool);

    is_attached = false;
    vDB = p_shm_t;
    if (vDB && vDB->v_ptr)
    {
        is_attached = true;
    }
}

vx_str::~vx_str ()
{
    vSTR_len = 0;

    if (v_token_list)
    {
        vtx_delete (v_token_list);
    }

    if (v_status == APR_SUCCESS && v_pool)
    {
        apr_pool_destroy(v_pool);
        v_pool = NULL;
    }
}

void *vx_str::ctx_malloc (PCRE2_SIZE pSIZE, void *pFUNC)
{
#if defined(USE_TCMALLOC)
    void *block = (void *)tc_malloc ((size_t)pSIZE);
#elif defined(USE_JEMALLOC)
    void *block = (void *) je_malloc((size_t) pSIZE);
#else
    void *block = (void *)malloc ((size_t)pSIZE);
#endif

    (void) pFUNC;

    return block;
}

void vx_str::ctx_free (void *pBLOCK, void *pFUNC)
{
    (void) pFUNC;

#if defined(USE_TCMALLOC)
    tc_free ((void *)pBLOCK);
#elif defined(USE_JEMALLOC)
    je_free((void *) pBLOCK);
#else
    free ((void *)pBLOCK);
#endif
}

void vx_str::context_create ()
{
    if (!vDB || !is_attached)
    {
        return;
    }
    if (!vDB->v_ptr)
    {
        return;
    }
    v_number_of_codes = pcre2_serialize_get_number_of_codes ((const uint8_t *)vDB->v_ptr);

    if (v_number_of_codes > 0)
    {
        v_ctx_general = pcre2_general_context_create(ctx_malloc, ctx_free, NULL);

        v_list_of_codes = (pcre2_code **) apr_palloc(v_pool, sizeof(pcre2_code *) * v_number_of_codes);

        v_name_count        = (int32_t *) apr_palloc(v_pool, sizeof(int32_t) * v_number_of_codes);
        v_name_table        = (PCRE2_SPTR *) apr_palloc(v_pool, sizeof(PCRE2_SPTR) * v_number_of_codes);
        v_name_entry_size   = (int32_t *) apr_palloc(v_pool, sizeof(int32_t) * v_number_of_codes);

        v_match_context     = (pcre2_match_context **) apr_palloc(v_pool, sizeof(pcre2_match_context *) * v_number_of_codes);
        v_jit_stack         = (pcre2_jit_stack **) apr_palloc(v_pool, sizeof(pcre2_jit_stack *) * v_number_of_codes);
        v_match_data        = (pcre2_match_data **) apr_palloc(v_pool, sizeof(pcre2_match_data *) * v_number_of_codes);
        v_out_vector        = (PCRE2_SIZE **) apr_palloc(v_pool, sizeof(PCRE2_SIZE *) * v_number_of_codes);

        pcre2_serialize_decode(v_list_of_codes, v_number_of_codes, (const uint8_t *)vDB->v_ptr, v_ctx_general);

        for (int32_t v_idx_code = 0; v_idx_code < v_number_of_codes; v_idx_code++)
        {
            pcre2_code *v_re = v_list_of_codes[v_idx_code];
            pcre2_jit_compile(v_re, PCRE2_JIT_COMPLETE);

            (void) pcre2_pattern_info(v_re, PCRE2_INFO_NAMECOUNT, &v_name_count[v_idx_code]);
            (void) pcre2_pattern_info(v_re, PCRE2_INFO_NAMETABLE, &v_name_table[v_idx_code]);
            (void) pcre2_pattern_info(v_re, PCRE2_INFO_NAMEENTRYSIZE, &v_name_entry_size[v_idx_code]);

            if (v_name_entry_max_size < v_name_entry_size[v_idx_code])
            {
                v_name_entry_max_size = v_name_entry_size[v_idx_code];
            }

            v_match_context[v_idx_code] = pcre2_match_context_create(NULL);
            pcre2_set_match_limit (v_match_context[v_idx_code], -1);
            pcre2_set_recursion_limit (v_match_context[v_idx_code], 1);

            v_jit_stack[v_idx_code] = pcre2_jit_stack_create(32 * 1024, 512 * 1024, NULL);
            pcre2_jit_stack_assign(v_match_context[v_idx_code], NULL, v_jit_stack[v_idx_code]);

            v_match_data[v_idx_code] = pcre2_match_data_create_from_pattern(v_re, NULL);
            v_out_vector[v_idx_code] = pcre2_get_ovector_pointer(v_match_data[v_idx_code]);
        }
#if 0
        re = pcre2_compile(
            (PCRE2_SPTR) pPATTERN,                   /* the pattern */
            (PCRE2_SIZE) PCRE2_ZERO_TERMINATED,      /* indicates pattern is zero-terminated */
            PCRE2_UCP | PCRE2_UTF | PCRE2_DUPNAMES | PCRE2_CASELESS | PCRE2_ALLOW_EMPTY_CLASS,      /* Option bits */
            &v_error_code,              /* for error code */
            &v_error_offset,            /* for error offset */
            v_ctx_compile);            /* use default compile context */

        if (re != NULL)
        {
            pcre2_jit_compile(re, PCRE2_JIT_COMPLETE);

            (void) pcre2_pattern_info(re, PCRE2_INFO_NAMECOUNT, &v_name_count);
            (void) pcre2_pattern_info(re, PCRE2_INFO_NAMETABLE, &v_name_table);
            (void) pcre2_pattern_info(re, PCRE2_INFO_NAMEENTRYSIZE, &v_name_entry_size);
        }
        else
        {
            PCRE2_UCHAR8 vBUF[512];
            (void) pcre2_get_error_message(v_error_code, vBUF, 512);
            std::cout << _INFO (v_pool) << __func__ << ":" << vBUF << std::endl << std::flush;
        }
#endif
    }
}

void vx_str::context_free ()
{
    for (int32_t v_idx_code = 0; v_idx_code < v_number_of_codes; v_idx_code++)
    {
        pcre2_code *v_re = v_list_of_codes[v_idx_code];
        pcre2_code_free (v_re);

        pcre2_match_data_free(v_match_data[v_idx_code]);   /* Release memory used for the match */
        pcre2_match_context_free(v_match_context[v_idx_code]);
        pcre2_jit_stack_free(v_jit_stack[v_idx_code]);
    }

    if (v_ctx_general)
    {
        pcre2_general_context_free(v_ctx_general);
    }
}

#if 0
void vx_str::read (const char *pSTR, int32_t pSTR_len)
{
    uSTR.remove();
    uSTR_len = 0;

    apr_pool_clear(v_pool_token);
    vSTR = NULL;
    vSTR_len = 0;

    uSTR = UnicodeString::fromUTF8(StringPiece((char *) pSTR, pSTR_len));
    uSTR_len = uSTR.length();

    std::string szUTF8_tmp("");

    uSTR.toLower().toUTF8String(szUTF8_tmp);

    vSTR = (const char *) apr_pstrdup(v_pool_token, szUTF8_tmp.c_str());
    vSTR_len = (int32_t) szUTF8_tmp.length();

    szUTF8_tmp.clear();
}

void vx_str::Text_normalize (const char *pSTR, int32_t pSTR_len, bool toLower)
{
    UErrorCode status = U_ZERO_ERROR;

    uSTR.remove();
    uSTR_len = 0;

    apr_pool_clear(v_pool_token);
    vSTR = NULL;
    vSTR_len = 0;

    const Normalizer2 &nNFC = *Normalizer2::getNFCInstance(status);
    if (U_SUCCESS(status))
    {
        UnicodeString uSTR_tmp = UnicodeString::fromUTF8(StringPiece((char *) pSTR, pSTR_len));

        status = U_ZERO_ERROR;
        uSTR = nNFC.normalize((toLower) ? uSTR_tmp.toLower() : uSTR_tmp, status);

        if (U_SUCCESS(status))
        {
            std::string szUTF8_tmp("");

            uSTR.toUTF8String(szUTF8_tmp);
            uSTR_len = uSTR.length();

            vSTR = (const char *) apr_pstrdup(v_pool_token, szUTF8_tmp.c_str());
            vSTR_len = (int32_t) szUTF8_tmp.length();

            szUTF8_tmp.clear();
        }
    }
}


void vx_str::normalize (const char *pSTR, int32_t pSTR_len)
{
    if (U_SUCCESS(v_error_code_normlzer))
    {
        uSTR.remove();
        uSTR_len = 0;

        apr_pool_clear(v_pool_token);
        vSTR = NULL;
        vSTR_len = 0;

        UnicodeString uSTR_raw = UnicodeString::fromUTF8(StringPiece((char *) pSTR, pSTR_len));

        UErrorCode status = U_ZERO_ERROR;
        uSTR = v_NFC->normalize((opt_toLowerCase) ? uSTR_raw.toLower() : uSTR_raw, status);

        if (U_SUCCESS(status))
        {
            std::string szUTF8_tmp("");

            uSTR.toUTF8String(szUTF8_tmp);
            uSTR_len = uSTR.length();

            vSTR = (const char *) apr_pstrdup(v_pool_token, szUTF8_tmp.c_str());
            vSTR_len = (int32_t) szUTF8_tmp.length();

            szUTF8_tmp.clear();
        }
    }
}
#endif

PCRE2_SPTR vx_str::proc_token_tag (int32_t p_idx_code, int32_t p_offset_utf8, int32_t p_len_utf8, const PCRE2_SIZE *p_vector)
{
    PCRE2_SPTR v_name = nullptr;
    PCRE2_SPTR p_table = v_name_table[p_idx_code];
    int32_t n = 0;
    int32_t v_offset_utf8_s = 0;    // 2 * n
    int32_t v_offset_utf8_e = 0;    // 2 * n + 1
    int32_t v_len_utf8 = 0;

    for (int32_t vIDX = 0; !v_name && vIDX < v_name_count[p_idx_code]; vIDX+=2)
    {
        if (!v_name && vIDX < v_name_count[p_idx_code])
        {
            n = (p_table[0] << 8) | p_table[1];
            v_offset_utf8_s = (int32_t) p_vector[2 * n];        // 2 * n
            v_offset_utf8_e = (int32_t) p_vector[2 * n + 1];    // 2 * n + 1
            v_len_utf8 = (int32_t) (v_offset_utf8_e - v_offset_utf8_s);

            if (v_offset_utf8_s == p_offset_utf8 && (v_len_utf8 > 0 && v_len_utf8 == p_len_utf8))
            {
                v_name = (PCRE2_SPTR) p_table + 2;

                break;
            }
            p_table += v_name_entry_size[p_idx_code];
        }
        if (!v_name && (vIDX + 1) < v_name_count[p_idx_code])
        {
            n = (p_table[0] << 8) | p_table[1];
            v_offset_utf8_s = (int32_t) p_vector[2 * n];        // 2 * n
            v_offset_utf8_e = (int32_t) p_vector[2 * n + 1];    // 2 * n + 1
            v_len_utf8 = (int32_t) (v_offset_utf8_e - v_offset_utf8_s);

            if (v_offset_utf8_s == p_offset_utf8 && (v_len_utf8 > 0 && v_len_utf8 == p_len_utf8))
            {
                v_name = (PCRE2_SPTR) p_table + 2;

                break;
            }
            p_table += v_name_entry_size[p_idx_code];
        }
    }

    return v_name;
}

#if 0
PCRE2_SPTR vx_str::proc_token_tag_all (int32_t p_idx_token, int32_t p_idx_code, int32_t p_offset_utf8, int32_t p_len_utf8, int32_t p_offset_ucs_s, const PCRE2_SIZE *p_vector)
{
    PCRE2_SPTR v_name = NULL;
    PCRE2_SPTR p_name = v_name_table[p_idx_code];
//    int32_t v_offset_utf8 = p_offset_utf8;
//    int32_t v_offset_ucs = p_offset_ucs_s;

    int32_t vFLAG = 1;
    for (int32_t vIDX = 0; vFLAG && vIDX < v_name_count[p_idx_code]; vIDX++)
    {
        int32_t n = (p_name[0] << 8) | p_name[1];
        int32_t v_offset_utf8_s = (int32_t) p_vector[2 * n];        // 2 * n
        int32_t v_offset_utf8_e = (int32_t) p_vector[2 * n + 1];    // 2 * n + 1
        int32_t v_len_utf8 = (int32_t) (v_offset_utf8_e - v_offset_utf8_s);

        if (v_len_utf8 > 0)
        {
            if (
                v_name == NULL && (v_offset_utf8_s == p_offset_utf8 && v_len_utf8 == p_len_utf8)
               )
            {
                v_name = (PCRE2_SPTR) p_name + 2;

                vFLAG = 0;
                break;
            }
            int32_t v_offset_utf8 = p_offset_utf8;
            int32_t v_offset_ucs = p_offset_ucs_s;
            PCRE2_SPTR v_ptr = (PCRE2_SPTR) vSTR + v_offset_utf8_s;
            UPDATE_UCS_offset (v_offset_utf8, v_offset_utf8_s, vSTR, (int32_t)vSTR_len, v_offset_ucs); // update UCS offset

            int32_t v_len_ucs = 0;
            UPDATE_UCS_len (v_ptr, v_len_utf8, v_len_ucs); // update ucs length

//            std::cout << apr_psprintf (v_pool, " (%d)", n);
//            std::cout << apr_psprintf (v_pool, "[%2d/%d]", vIDX + 1, v_name_count);
            std::cout << apr_psprintf(v_pool, "%6d %5d(%3d) %5d(%3d)",
                p_idx_token,
                (int32_t) v_offset_ucs, (int32_t) v_len_ucs,
                (int32_t) v_offset_utf8_s, (int32_t) v_len_utf8
            );
            std::cout << apr_psprintf(v_pool, " [%*s]",
                v_name_entry_max_size - 3,
                (char *) (p_name + 2));
            std::cout << apr_psprintf(v_pool, " [%.*s]",
                (int32_t) v_len_utf8, v_ptr);
            std::cout << std::endl;
        }
        p_name += v_name_entry_size[p_idx_code];
    }

    return v_name;
}
#endif

void vx_str::tokenize (const char *pSTR, size_t pSTR_len)
{
    if (!v_ctx_general || !v_number_of_codes)
    {
        return;
    }
    int32_t v_idx_token = 0;
    int32_t v_offset_utf8 = 0;
    int32_t v_offset_ucs = 0;
    int32_t v_offset_ucs_prev = -1;
    int32_t v_idx_sgmt = 0;
    int32_t v_idx_elt = 0;
    int32_t vFLAG = 1;

    vSTR = pSTR;
    vSTR_len = pSTR_len;
    if (v_token_list)
    {
        vtx_clear(v_token_list);
    }
    else
    {
        v_token_list = vtx_create(v_pool);
    }

    int32_t vIDX_code = v_number_of_codes - 1;
    pcre2_code *v_re = v_list_of_codes[vIDX_code];
    for (; vFLAG && v_offset_utf8 < (int32_t)vSTR_len;)
    {
        uint32_t v_options = 0;                    /* Normally no options */
        int32_t rc = pcre2_jit_match(
            v_re,                      /* the compiled pattern */
            (PCRE2_SPTR) vSTR,       /* the subject string */
            (size_t) vSTR_len,       /* the length of the subject */
            (size_t) v_offset_utf8,  /* start at offset 0 in the subject */
            v_options,               /* default options */
            v_match_data[vIDX_code],              /* block for storing the result */
            v_match_context[vIDX_code]);              /* use default match context */

        if (rc == PCRE2_ERROR_NOMATCH)
        {
            vFLAG = 0;
            break;
        }
        else if (rc > 0)
        {
            int32_t v_offset_utf8_s = (int32_t) v_out_vector[vIDX_code][0];  // 2 * vIDX
            int32_t v_offset_utf8_e = (int32_t) v_out_vector[vIDX_code][1];  // 2 * vIDX + 1
            int32_t v_len_utf8 = v_offset_utf8_e - v_offset_utf8_s;
            if (v_offset_utf8_e > 0 && v_len_utf8 > 0)
            {
                PCRE2_SPTR v_ptr = (PCRE2_SPTR) vSTR + v_offset_utf8_s;
                int32_t v_offset_ucs_s = v_offset_ucs;
                int32_t v_len_ucs = 0;

                UPDATE_UCS_offset (v_offset_utf8, v_offset_utf8_s, vSTR, (int32_t)vSTR_len, v_offset_ucs_s); // update UCS offset
                v_offset_ucs = v_offset_ucs_s;
                v_offset_utf8 = v_offset_utf8_e;

                UPDATE_UCS_len (v_ptr, v_len_utf8, v_len_ucs); // update ucs length
                v_offset_ucs += v_len_ucs;

                PCRE2_SPTR v_name = proc_token_tag(vIDX_code, v_offset_utf8_s, v_len_utf8, (const PCRE2_SIZE *) v_out_vector[vIDX_code]);
                if (v_offset_ucs_prev != -1 && v_offset_ucs_prev != v_offset_ucs_s)
                {
                    v_idx_sgmt++;
                    v_idx_elt = 0;
                }
#if 0
                std::cout << apr_psprintf(v_pool, "%6d [%6d] <%5d>%5d(%3d) %5d(%3d)",
                    v_idx_sgmt,
                    v_idx_token,
                    v_offset_ucs_prev,
                    (int32_t) v_offset_ucs_s, (int32_t) v_len_ucs,
                    (int32_t) v_offset_utf8_s, (int32_t) v_len_utf8
                );
                std::cout
                    << apr_psprintf(v_pool, " [%*s]", v_name_entry_max_size - 3, (v_name == NULL) ? (char *) _TPL_DEFAULT_TAG : (char *) v_name);
                std::cout << apr_psprintf(v_pool, " [%.*s]", (int32_t) v_len_utf8, (char *) v_ptr);
                std::cout << std::endl;
#endif
                vtx_push_back (v_token_list, v_idx_token,
                    v_idx_sgmt,
                    v_idx_elt,
                    (const char *) v_ptr, v_len_utf8,
                    v_offset_ucs_s, v_len_ucs,
                    v_offset_utf8_s, v_len_utf8,
                    (v_name == NULL) ? _TPL_DEFAULT_TAG : (const char *) v_name);

                v_offset_ucs_prev = v_offset_ucs;
                v_idx_token++;
                v_idx_elt++;
            } // if v_offset_utf8_e > 0
        } // if else
        else
        {
            std::cout << ">>> " << __LINE__ << ":" << v_offset_utf8 << ":" << rc << std::endl;

            vFLAG = 0;
            break;
        }
    } // for
}

void vx_str::print ()
{
    if (v_token_list)
    {
        vtx_print(v_token_list);
    }
}

const char * vx_str::dumps_text ()
{
    const char *v_buffer = nullptr;

    if (vDB && is_attached && v_token_list)
    {
        v_buffer = vtx_text_print(v_token_list);
    }

    return v_buffer;
}

const char * vx_str::dumps_json ()
{
    const char *v_buffer = nullptr;

    if (vDB && is_attached && v_token_list)
    {
        v_buffer = vtx_json_print(v_token_list);
    }

    return v_buffer;
}

#if 0
void vx_str::tokenize_2 ()
{
    int32_t v_offset_utf8 = 0;
    int32_t v_offset_ucs = 0;
    int32_t vFLAG = 1;

    int32_t vIDX_ = v_number_of_codes - 1;
    int32_t *flag_code = new int32_t[vIDX_];
    for (int32_t vIDX = 0; vIDX < vIDX_; vIDX++)
    {
        flag_code[vIDX] = 1;
    }

    while (vFLAG && v_offset_utf8 < vSTR_len)
    {
//        std::cout << _INFO (v_pool) << apr_psprintf(v_pool, "%4d/%d", v_offset_utf8, vSTR_len) << std::endl << std::flush;
        int32_t v_offset_last_utf8 = vSTR_len;

        int32_t l_idx = -1;
        int32_t l_offset_ucs_s = 0;
        int32_t l_offset_utf8_s = 0;
        int32_t l_len_ucs = 0;
        int32_t l_len_utf8 = 0;
        for (int32_t v_idx_code = 0; v_idx_code < v_number_of_codes; v_idx_code++)
        {
            if (flag_code[v_idx_code] == 0)
            {
                continue;
            }
            pcre2_code *v_re = v_list_of_codes[v_idx_code];
            int32_t t_offset_utf8 = v_offset_utf8;
            uint32_t v_options = 0;                    /* Normally no options */
            int32_t rc = pcre2_jit_match(
                v_re,                      /* the compiled pattern */
                (PCRE2_SPTR) vSTR,       /* the subject string */
                (size_t) vSTR_len,       /* the length of the subject */
                (size_t) t_offset_utf8,  /* start at offset 0 in the subject */
                v_options,               /* default options */
                v_match_data[v_idx_code],              /* block for storing the result */
                v_match_context[v_idx_code]);              /* use default match context */

            if (rc == PCRE2_ERROR_NOMATCH)
            {
                flag_code[v_idx_code] = 0;
                continue;
            }
            else if (rc > 0)
            {
                int32_t v_offset_utf8_s = (int32_t) v_out_vector[v_idx_code][0];  // 2 * vIDX
                int32_t v_offset_utf8_e = (int32_t) v_out_vector[v_idx_code][1];  // 2 * vIDX + 1
                int32_t v_len_utf8 = v_offset_utf8_e - v_offset_utf8_s;
                if (v_offset_utf8_e > 0 && v_len_utf8 > 0)
                {
                    PCRE2_SPTR v_ptr = (PCRE2_SPTR) vSTR + v_offset_utf8_s;
                    int32_t v_offset_ucs_s = v_offset_ucs;
                    int32_t v_len_ucs = 0;

                    UPDATE_UCS_offset (t_offset_utf8, v_offset_utf8_s, vSTR, (int32_t)vSTR_len, v_offset_ucs_s); // update UCS offset

                    UPDATE_UCS_len (v_ptr, v_len_utf8, v_len_ucs); // update ucs length

                    if (t_offset_utf8 < v_offset_last_utf8)
                    {
                        l_idx = v_idx_code;
                        v_offset_last_utf8 = v_offset_utf8_s;
                        l_offset_utf8_s = v_offset_utf8_s;
                        l_offset_ucs_s = v_offset_ucs_s;
                        l_len_utf8 = v_len_utf8;
                        l_len_ucs = v_len_ucs;
                        //std::cout << _INFO (v_pool) << apr_psprintf(v_pool, "%4d/%d", v_offset_utf8_s, vSTR_len) << std::endl << std::flush;
                    }
                } // if v_offset_utf8_e > 0
            } // if else
        } // for
        v_offset_utf8 = l_offset_utf8_s + l_len_utf8;
        v_offset_ucs = l_offset_ucs_s + l_len_ucs;
        if (l_idx >= 0)
        {
            PCRE2_SPTR v_ptr = (PCRE2_SPTR) vSTR + l_offset_utf8_s;

            PCRE2_SPTR v_name = proc_token_tag(l_idx, l_offset_utf8_s, l_len_utf8, l_offset_ucs_s, (const PCRE2_SIZE *) v_out_vector[l_idx]);

            if (v_name == NULL)
            {
                std::cout << apr_psprintf(v_pool, "%5d(%3d) %5d(%3d)",
                    (int32_t) l_offset_ucs_s, (int32_t) l_len_ucs,
                    (int32_t) l_offset_utf8_s, (int32_t) l_len_utf8
                );
                std::cout
                    << apr_psprintf(v_pool, " (%*s)", v_name_entry_max_size - 3, (v_name == NULL) ? (char *) "_PHRASE" : (char *) v_name);
                std::cout << apr_psprintf(v_pool, " [%.*s]", (int32_t) l_len_utf8, (char *) v_ptr);
                std::cout << std::endl;
            }
        }
        else{
            vFLAG = 0;
            break;
        }
    } // while

    delete flag_code;
}
#endif
