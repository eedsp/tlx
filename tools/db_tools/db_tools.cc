///
// Created by Shiwon Cho on 2005.08.20.
//
#include <iostream>


#include <apr_general.h>
#include <jansson.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include <cctype>
#include <ctime>

#include "vx_str.h"
#include "db_tools.h"

//const int32_t v_filename_width = 50;

static void print_file_info (apr_pool_t *p_pool, v_config_t *pCFG, int32_t pIDX, int32_t pSIZE, const char *pFILE)
{
    if (pCFG->v_debug)
    {
#if 0
        std::cout << _INFO (p_pool) << apr_psprintf(p_pool, "FILE: %4d/%d: ", pIDX, (int32_t) pSIZE);
        if (std::strlen(pFILE) > v_filename_width)
        {
            std::cout << apr_psprintf(p_pool, "%.*s...%s",
                (v_filename_width / 2) - 3,
                pFILE,
                (std::strlen(pFILE) - (v_filename_width / 2)) + pFILE);
        }
        else
        {
            std::cout << apr_psprintf(p_pool, "%*s", v_filename_width, pFILE);
        }
        std::cout << std::endl << std::flush;
#else
        std::cout << _INFO (p_pool) << apr_psprintf(p_pool, "FILE: %4d/%d: %s", pIDX, (int32_t) pSIZE, pFILE);
        std::cout << std::endl << std::flush;
#endif
    }
}

static int32_t check_pattern (apr_pool_t *p_pool, int32_t pLINE, const char *pPATTERN)
{
    int32_t ret_t = HX_OK;
    PCRE2_SIZE v_error_offset;
    int32_t v_error_code;

    pcre2_compile_context *v_ctx_compile = pcre2_compile_context_create(NULL);
    pcre2_code *re = pcre2_compile(
        (PCRE2_SPTR) pPATTERN,        /* the pattern */
        (PCRE2_SIZE) PCRE2_ZERO_TERMINATED,      /* indicates pattern is zero-terminated */
        PCRE2_UCP | PCRE2_UTF | PCRE2_DUPNAMES | PCRE2_CASELESS,      /* Option bits */
        &v_error_code,              /* for error code */
        &v_error_offset,            /* for error offset */
        v_ctx_compile);            /* use default compile context */

    if (re != NULL)
    {
        pcre2_code_free(re);
        pcre2_compile_context_free(v_ctx_compile);
    }
    else
    {
        PCRE2_UCHAR8 vBUF[512];
        (void) pcre2_get_error_message(v_error_code, vBUF, 512);
        std::cout << _INFO (p_pool) << pLINE << " : " << pPATTERN << std::endl << std::flush;
        std::cout << _INFO (p_pool) << "ERROR  : " << vBUF << std::endl << std::flush;

        ret_t = HX_ERROR;
    }
    return ret_t;
}

static void proc_file_read (apr_pool_t *p_pool, v_config_t *pCFG, const char *v_file)
{
    std::ifstream is;
    UErrorCode u_status = U_ZERO_ERROR;

    const Normalizer2 &nNFC = *Normalizer2::getNFCInstance(u_status);

    if (U_SUCCESS(u_status))
    {
        int32_t v_idx = 0;
        int32_t v_line = 0;
        is.open(v_file, std::ios::binary);
        while (is.good())
        {
            std::string szLINE;
            getline(is, szLINE);

            v_line++;
            UnicodeString uSTR_tmp = UnicodeString::fromUTF8(StringPiece(szLINE.c_str()));
            uSTR_tmp.trim();
            UnicodeString uSTR = nNFC.normalize(uSTR_tmp, u_status);

            if (U_SUCCESS(u_status))
            {
                std::string szSTR_tmp("");
                uSTR.toUTF8String(szSTR_tmp);

                std::string::const_reference crefStr = szSTR_tmp[0];
                if (crefStr == '#')
                {
                    continue;
                }
                if (szSTR_tmp.length() > 0)
                {
                    int32_t rc_t = check_pattern(p_pool, v_line, szSTR_tmp.c_str());
                    if (rc_t == HX_OK)
                    {
                        if (pCFG->v_debug)
                        {
                            std::cout << _INFO (p_pool) << "PASS   : " << szSTR_tmp.c_str() << std::endl << std::flush;
                        }
                        pCFG->v_str.push_back(std::string(szSTR_tmp));
                        v_idx++;
                    }
                }
            }
        }
        is.close();
    }
}

static apr_status_t proc_mkdir (apr_pool_t *p_pool, v_config_t *pCFG, const char *pPATH)
{
    apr_status_t v_status = -1;
    if (pPATH != NULL)
    {
        if (pCFG->v_debug)
        {
            std::cout << _INFO (p_pool) << "Create directory: " << pPATH << std::endl << std::flush;
        }
        v_status = apr_dir_make_recursive(pPATH,
            APR_UREAD | APR_UWRITE | APR_UEXECUTE | APR_GREAD | APR_GWRITE | APR_GEXECUTE | APR_WREAD | APR_WWRITE | APR_WEXECUTE,
            p_pool);
    }

    return v_status;
}

static apr_status_t proc_file_save (apr_pool_t *p_pool, v_config_t *pCFG, const char *pPATH, const char *pFILE, const void *pDATA, apr_size_t pDATA_len)
{
    apr_status_t v_status = -1;
    if (pFILE != NULL)
    {
        apr_file_t *v_fp = NULL;
        const char *vFILE = (const char *) (pPATH != NULL) ? apr_psprintf(p_pool, "%s/%s", pPATH, pFILE) : pFILE;

        if (pCFG->v_debug)
        {
            std::cout << _INFO (p_pool) << "Create file: " << vFILE << std::endl << std::flush;
        }
        if ((v_status = apr_file_open(&v_fp, vFILE,
            APR_WRITE | APR_CREATE | APR_BINARY | APR_FOPEN_TRUNCATE | APR_FOPEN_BUFFERED,
            APR_OS_DEFAULT, p_pool)) == APR_SUCCESS)
        {
            apr_size_t v_fsize = 0;
            v_status = apr_file_write_full(v_fp, pDATA, pDATA_len, (apr_size_t *) &v_fsize);

//            std::cout << _INFO (p_pool) << pDATA_len << ":" << v_fsize << " " << v_status << std::endl << std::flush;

            apr_file_close(v_fp);
        }
    }

    return v_status;
}

static int32_t proc_phrase (apr_pool_t *p_pool, v_config_t *pCFG)
{
    apr_pool_t *v_pool = NULL;
    apr_status_t v_mp_status = APR_SUCCESS;
    apr_status_t v_fp_status = APR_SUCCESS;

    int32_t v_idx = 0;

    v_mp_status = apr_pool_create (&v_pool, p_pool);
    if (v_mp_status == APR_SUCCESS)
    {
        pCFG->v_str.clear ();

        int32_t v_file_size = pCFG->v_files.size();
        if (v_file_size > 0)
        {
            v_idx = 0;
            for (auto it = pCFG->v_files.begin(); it != pCFG->v_files.end(); ++it)
            {
                std::string v_file = std::string(*it);
                const char *pFILE = (const char *) (pCFG->iPATH != NULL) ? apr_psprintf(v_pool, "%s/%s", pCFG->iPATH, v_file.c_str()) : v_file.c_str();

                print_file_info(v_pool, pCFG, v_idx + 1, v_file_size, pFILE);

                proc_file_read(v_pool, pCFG, pFILE);

                v_idx++;
            }
        }
        int32_t v_codes = (int32_t)pCFG->v_str.size ();
        std::cout << ">>> Pattern: " << v_codes << std::endl << std::flush;

        v_codes = 1;

        std::string *vSTR = new std::string("");

        PCRE2_SIZE v_error_offset = 0;
        int32_t v_error_code = 0;
        pcre2_code **list_of_codes = (pcre2_code **)apr_palloc(v_pool, sizeof(pcre2_code *) *v_codes);

        v_idx = 0;
        for (auto it = pCFG->v_str.begin(); it != pCFG->v_str.end(); ++it)
        {
            std::string v_pattern = std::string(*it);
#if 0
            list_of_codes[v_idx] = pcre2_compile(
                (PCRE2_SPTR) v_pattern,        /* the pattern */
                (PCRE2_SIZE) PCRE2_ZERO_TERMINATED,      /* indicates pattern is zero-terminated */
                PCRE2_UCP | PCRE2_UTF | PCRE2_DUPNAMES | PCRE2_CASELESS,      /* Option bits */
                &v_error_code,              /* for error code */
                &v_error_offset,            /* for error offset */
                NULL);                      /* use default compile context */
#endif
            if (pCFG->v_debug)
            {
                std::cout << _INFO (v_pool)
                    << apr_psprintf(v_pool, "%4d/%d", v_idx + 1, v_codes)
                    << ": " << v_pattern.c_str() << std::endl << std::flush;
            }
            if (v_idx > 0)
            {
                vSTR->append("|");
            }
            vSTR->append(v_pattern);

            v_idx++;
        }
        v_idx = 0;
        int32_t rc_t = check_pattern(p_pool, 1, vSTR->c_str());
        if (rc_t == HX_OK)
        {
            list_of_codes[v_idx] = pcre2_compile(
                (PCRE2_SPTR) vSTR->c_str(),        /* the pattern */
                (PCRE2_SIZE) PCRE2_ZERO_TERMINATED,      /* indicates pattern is zero-terminated */
                PCRE2_UCP | PCRE2_UTF | PCRE2_DUPNAMES | PCRE2_CASELESS,      /* Option bits */
                &v_error_code,              /* for error code */
                &v_error_offset,            /* for error offset */
                NULL);                      /* use default compile context */

            uint8_t *v_bytes = NULL;
            PCRE2_SIZE v_bytescount = 0;
            v_error_code = pcre2_serialize_encode((const pcre2_code **)list_of_codes, v_codes, &v_bytes,
                &v_bytescount, NULL);

            proc_mkdir (v_pool, pCFG, pCFG->tPATH);
            v_fp_status = proc_file_save(v_pool, pCFG, pCFG->tPATH, pCFG->tFILE, (const void *)vSTR->c_str(), (apr_size_t)vSTR->length());

            if (v_bytes != NULL && v_bytescount > 0)
            {
                proc_mkdir(v_pool, pCFG, pCFG->oPATH);

                v_fp_status = proc_file_save(v_pool, pCFG, pCFG->oPATH, pCFG->oFILE, (const void *) v_bytes, (apr_size_t) v_bytescount);

                if (v_fp_status == APR_SUCCESS)
                {
                    std::cout << ">>> Number of codes: " << pcre2_serialize_get_number_of_codes(v_bytes) << std::endl << std::flush;
                    std::cout << ">>> Byte count: " << v_bytescount << std::endl << std::flush;
                }
                pcre2_serialize_free(v_bytes);
            }

            std::cout << ">>> Done" << std::endl << std::flush;
        }
        for (int32_t v_idx = 0; v_idx < v_codes; v_idx++)
        {
            pcre2_code_free(list_of_codes[v_idx]);
        }

        delete vSTR;

        apr_pool_destroy(v_pool);
    }

    return HX_OK;
}

int32_t main (int32_t argc, const char *argv[])
{
    apr_status_t apr_s = apr_initialize();
    apr_pool_t *v_pool = NULL;

    v_config_t vCFG;

    // create json "objects" and use the json_t pointer to point to them
#if defined(USE_TCMALLOC)
    json_set_alloc_funcs(tc_malloc, tc_free);
#elif defined(USE_JEMALLOC)
    json_set_alloc_funcs(je_malloc, je_free);
#endif

    if (apr_s == APR_SUCCESS)
    {
        apr_pool_create(&v_pool, NULL);

        hx_status_t v_status = get_options(v_pool, argc, argv, &vCFG);
        if (v_status == HX_OK)
        {
            v_status = HX_ERROR;
            if (vCFG.conf_filename != NULL)
            {
                v_status = load_config(v_pool, &vCFG);
            }
            if (!vCFG.v_test_conf)
            {
                if (v_status == HX_OK)
                {
                    if (!apr_strnatcmp(vCFG.v_app, "phrase"))
                    {
                        proc_phrase(v_pool, &vCFG);
                    }
                }
            }

            if (vCFG.v_json)
            {
                json_decref(vCFG.v_json);
            }
        }

        apr_pool_destroy(v_pool);
    }
    apr_terminate();

    return 0;
}

