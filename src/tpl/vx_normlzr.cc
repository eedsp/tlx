//
// Created by Shiwon Cho on 2005.10.27.
//

#include <iostream>
#include "vx_normlzr.h"

vx_normlzr::vx_normlzr (apr_pool_t *p_pool)
{
    v_pool = NULL;

    uSTR.remove();
    uSTR_len = 0;
    vSTR = NULL;
    vSTR_len = 0;

    opt_toLowerCase = false;

    v_status = APR_SUCCESS;
    v_status = apr_pool_create (&v_pool, p_pool);
}

vx_normlzr::~vx_normlzr ()
{
    uSTR.remove();
    uSTR_len = 0;
    vSTR_len = 0;

    if (v_status == APR_SUCCESS && v_pool != NULL)
    {
        apr_pool_destroy(v_pool);
        v_pool = NULL;
    }
}

void vx_normlzr::normalize (const char *pSTR, size_t pSTR_len)
{
    uSTR.remove();
    uSTR_len = 0;

    apr_pool_clear(v_pool);
    vSTR = nullptr;
    vSTR_len = 0;

    if (pSTR_len > 0)
    {
        UErrorCode v_error_code_normlzer= U_ZERO_ERROR;
        const icu::Normalizer2 &v_NFC = *icu::Normalizer2::getNFCInstance(v_error_code_normlzer);

        if (U_SUCCESS(v_error_code_normlzer))
        {
            icu::UnicodeString uSTR_src = icu::UnicodeString::fromUTF8(icu::StringPiece((char *) pSTR, (int32_t)pSTR_len));

            UErrorCode status = U_ZERO_ERROR;
            uSTR = v_NFC.normalize((opt_toLowerCase) ? uSTR_src.toLower() : uSTR_src, status);

            if (U_SUCCESS(status))
            {
                std::string szUTF8_tmp("");

                uSTR.toUTF8String(szUTF8_tmp);
                uSTR_len = (size_t)uSTR.length();

                vSTR = (const char *) apr_pstrdup(v_pool, szUTF8_tmp.c_str());
                vSTR_len = (size_t) szUTF8_tmp.length();

                szUTF8_tmp.clear();
            }
        }
    }
}

extern "C" const char * TEXT_normalize (apr_pool_t *p, const char *pSTR, size_t pSTR_len, size_t *v_STR_len, int32_t pOPT)
{
    std::string szUTF8("");
    const char *v_STR = nullptr;
    *v_STR_len = 0;

    icu::UnicodeString uSTR_ucs = icu::UnicodeString::fromUTF8(icu::StringPiece((const char *) pSTR, (int32_t)pSTR_len));

    UErrorCode status = U_ZERO_ERROR;
    const icu::Normalizer2 &nNFC = *icu::Normalizer2::getNFCInstance(status);
    if (U_SUCCESS(status))
    {
        status = U_ZERO_ERROR;
        icu::UnicodeString usNFC = nNFC.normalize((pOPT == 0) ? uSTR_ucs : uSTR_ucs.toLower(), status);
        if (U_SUCCESS(status))
        {
            usNFC.toUTF8String(szUTF8);
            usNFC.remove();

            v_STR = apr_pstrdup(p, szUTF8.c_str());
            *v_STR_len = (size_t)szUTF8.length();
        }
    }
    szUTF8.clear();

    return v_STR;
}
