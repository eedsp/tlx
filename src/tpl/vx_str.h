//
// Created by Shiwon Cho on 2005.10.27.
//
#pragma once

#if defined(USE_JEMALLOC)
  #include <jemalloc/jemalloc.h>
#endif

#if !defined(U_CHARSET_IS_UTF8)
#define U_CHARSET_IS_UTF8 1
#endif

#include <sys/shm.h>
#include <unicode/utypes.h>
#include <unicode/utf8.h>
#include <unicode/uchar.h>
#include <unicode/stringpiece.h>
#include <unicode/unistr.h>
#include <unicode/normalizer2.h>

#include <apr_pools.h>
#include <apr_strings.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include "tlx_types.h"
#include "hx_log.h"
#include "hx_util.h"

#include "vx_type.h"
#include "vtx.h"

class vx_str
{
  private:
    apr_pool_t *v_pool;
    apr_status_t v_status;
    //    apr_pool_t *v_pool_token;
    //    apr_status_t v_status_token;

    int32_t v_number_of_codes;
    pcre2_code **v_list_of_codes;
    pcre2_general_context *v_ctx_general;
    PCRE2_SIZE v_error_offset;
    int32_t v_error_code;

    int32_t *v_name_count;
    int32_t *v_name_entry_size;
    int32_t v_name_entry_max_size;
    PCRE2_SPTR *v_name_table;

    pcre2_match_context **v_match_context;
    pcre2_jit_stack **v_jit_stack;
    pcre2_match_data **v_match_data;
    PCRE2_SIZE **v_out_vector;

    const char *vSTR;
    size_t vSTR_len;

    bool is_attached;
    const hx_shm_rec_t *vDB;

    vtx_list_t *v_token_list;

    static void *ctx_malloc (PCRE2_SIZE pSIZE, void *pFUNC);
    static void ctx_free (void *pBLOCK, void *pFUNC);

    PCRE2_SPTR proc_token_tag (int32_t p_idx_code, int32_t p_offset_utf8, int32_t p_len_utf8, const PCRE2_SIZE *p_vector);
    //PCRE2_SPTR proc_token_tag_all (int32_t p_idx_token, int32_t p_idx_code, int32_t p_offset_utf8, int32_t p_len_utf8, int32_t p_offset_ucs_s, const PCRE2_SIZE *p_vector);

  public:
    vx_str (apr_pool_t *p_pool, const hx_shm_rec_t *p_shm_t);
    ~vx_str ();

    void context_create ();
    void context_free ();
    void tokenize (const char *pSTR, size_t pSTR_len);
    void print ();
    const char *dumps_text ();
    const char *dumps_json ();
    //    void tokenize_2 ();
};
