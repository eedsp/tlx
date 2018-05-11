//
// Created by Shiwon Cho on 2005.10.27.
//

#pragma once

#if !defined(U_CHARSET_IS_UTF8)
#define U_CHARSET_IS_UTF8 1
#endif

#include <unicode/utypes.h>
#include <unicode/utf8.h>
#include <unicode/uchar.h>
#include <unicode/stringpiece.h>
#include <unicode/unistr.h>
#include <unicode/normalizer2.h>

#include <apr_pools.h>
#include <apr_strings.h>

#include "tlx_types.h"

class vx_normlzr
{
  private:
    apr_pool_t *v_pool;
    apr_status_t v_status;

  public:
    vx_normlzr (apr_pool_t *p_pool);
    ~vx_normlzr ();

    icu::UnicodeString uSTR;
    size_t uSTR_len;
    const char *vSTR;
    size_t vSTR_len;

    bool opt_toLowerCase;

    void normalize (const char *pSTR, size_t pSTR_len);
};

#ifdef __cplusplus
extern "C" {
#endif
const char *TEXT_normalize (apr_pool_t *p, const char *pSTR, size_t pSTR_len, size_t *v_STR_len, int32_t pOPT);
#ifdef __cplusplus
}
#endif

