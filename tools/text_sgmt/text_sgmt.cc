///
// Created by Shiwon Cho on 2005.08.20.
//
#include <iostream>

#if defined(USE_JEMALLOC)
  #include <jemalloc/jemalloc.h>
#endif

#include <sys/shm.h>

#include <apr_general.h>
#include <apr_file_io.h>
#include <jansson.h>

#include "vx_normlzr.h"
#include "vx_str.h"
#include "text_sgmt_opt.h"

const int32_t v_filename_width = 50;

#if 0
static const void *proc_file_read (apr_pool_t *p_pool, v_config_t *pCFG)
{
    apr_file_t *v_fp = NULL;
    apr_status_t v_status = -1;
    apr_finfo_t v_finfo;
    const void *szDATA = NULL;

    const char *v_file = apr_psprintf(p_pool, "%s/%s", pCFG->db_path, pCFG->db_phrase);
    if ((v_status = apr_file_open(&v_fp, v_file, APR_READ | APR_BUFFERED | APR_BINARY, APR_OS_DEFAULT, p_pool)) == APR_SUCCESS)
    {
        v_status = apr_file_info_get(&v_finfo, APR_FINFO_NORM, v_fp);

        apr_size_t v_fsize = 0;
        szDATA = (const void *) apr_palloc(p_pool, (apr_size_t) v_finfo.size + 1);

        apr_file_read_full(v_fp, (void *) szDATA, v_finfo.size, (apr_size_t *) &v_fsize);

        apr_file_close(v_fp);
    }

    return szDATA;
}
#endif

static void proc_file (apr_pool_t *p_pool, v_config_t *pCFG, const char *v_file, vx_normlzr *t_normlzr, vx_str *t_str)
{
    apr_pool_t *v_pool = NULL;
    apr_file_t *v_fp = NULL;
    apr_status_t v_status;
    apr_finfo_t v_finfo;

    v_status = apr_pool_create (&v_pool, p_pool);

    if (v_status == APR_SUCCESS)
    {
        if ((v_status = apr_file_open(&v_fp, v_file, APR_READ | APR_BUFFERED | APR_BINARY, APR_OS_DEFAULT, v_pool)) == APR_SUCCESS)
        {
            v_status = apr_file_info_get(&v_finfo, APR_FINFO_NORM, v_fp);

            apr_size_t v_fsize = 0;
            const char *szDATA = (const char *) apr_palloc(v_pool, (apr_size_t) v_finfo.size + 1);

            apr_file_read_full(v_fp, (void *) szDATA, v_finfo.size, (apr_size_t *) &v_fsize);

            double t1 = (double) s_clock();
            double t2 = 0;

            t_normlzr->normalize(szDATA, v_fsize);

            t_str->tokenize(t_normlzr->vSTR, t_normlzr->vSTR_len);

            t_str->print();

            t2 = (double) s_clock();

            std::cout << ">>> TIME: " << apr_psprintf(v_pool, "%f msec", double(t2 - t1)) << std::endl;
#if 0
            if (pCFG->v_debug)
            {
                std::cout << "TIME: " << apr_psprintf (v_pool, "%f msec", double (t2 - t1)) << std::endl;
                std::cout << "TIME: " << apr_psprintf (v_pool, "%f seconds", double (t2 - t1) / 1000) << std::endl;
                std::cout << "TIME: " << apr_psprintf (v_pool, "%f mins", (double(t2 - t1) / 1000) / 60) << std::endl;
            }
#endif

            apr_file_close(v_fp);
        }

        apr_pool_destroy(v_pool);
    }
}

static void print_file_info (apr_pool_t *p_pool, v_config_t *pCFG, int32_t pIDX, int32_t pSIZE, const char *pFILE)
{
    if (pCFG->v_debug)
    {
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
    }
}

static int32_t proc_file_list (apr_pool_t *p_pool, v_config_t *pCFG)
{
    apr_pool_t *v_pool = NULL;
    apr_status_t v_mp_status = APR_SUCCESS;

    v_mp_status = apr_pool_create (&v_pool, p_pool);
    if (v_mp_status == APR_SUCCESS)
    {

        if (pCFG->v_files.size () > 0)
        {
            int32_t v_idx = 0;

            const char *pFILE = apr_psprintf (v_pool, "%s/%s", pCFG->db_path, pCFG->db_phrase);

            vx_normlzr *t_normlzr = new vx_normlzr (v_pool);

            hx_shm_rec_t *v_shm_t = hx_shm_attach (v_pool, pFILE);
            vx_str *t_str = new vx_str(v_pool, v_shm_t);

            t_str->context_create();
            for (auto it = pCFG->v_files.begin(); it != pCFG->v_files.end(); ++it)
            {
                std::string v_file = std::string(*it);
                print_file_info(v_pool, pCFG, v_idx + 1, (int32_t) pCFG->v_files.size(), (const char *)v_file.c_str ());

                std::cout << apr_psprintf(p_pool,
                    ">> [%3d] ------------------------------------------------------------------------------------------------------",
                    v_idx + 1);
                std::cout << std::endl;

                proc_file(v_pool, pCFG, (const char *)v_file.c_str (), t_normlzr, t_str);

                std::cout << apr_psprintf(p_pool,
                    "<< [%3d] -----------------------------------------------------------------------------------------------------",
                    v_idx + 1);
                std::cout << std::endl;

                v_idx++;
            }

            t_str->context_free();

            hx_shm_detach (v_shm_t);

            delete t_str;
            delete t_normlzr;
        }


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

            if (v_status == HX_OK)
            {
                double t1 = (double) s_clock();

                proc_file_list(v_pool, &vCFG);

                double t2 = (double) s_clock();

                std::cout << "--------------------------------------------------------------------------------------------------------------"
                    << std::endl;
                std::cout << "TIME: " << apr_psprintf(v_pool, "%f msec", double(t2 - t1)) << std::endl;
                std::cout << "TIME: " << apr_psprintf(v_pool, "%f seconds", double(t2 - t1) / 1000) << std::endl;
                std::cout << "TIME: " << apr_psprintf(v_pool, "%f mins", (double(t2 - t1) / 1000) / 60) << std::endl;
                std::cout << "--------------------------------------------------------------------------------------------------------------"
                    << std::endl;
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

