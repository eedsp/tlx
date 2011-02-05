//
// Created by Shiwon Cho on 2005.10.28.
//
#include <thread>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/time.h>
#include <sys/shm.h>
#include <apr_pools.h>
#include <apr_strings.h>
#include <apr_uri.h>
#include <apr_md5.h>
#include "hx_util.h"

//  Return current system clock as milliseconds
extern "C" int64_t s_clock ()
{
#if (defined (__WINDOWS__))
	SYSTEMTIME st;
	GetSystemTime (&st);
	return (int64_t) st.wSecond * 1000 + st.wMilliseconds;
#else
	struct timeval tv;
	gettimeofday (&tv, NULL);
	return (int64_t) (tv.tv_sec * 1000 + tv.tv_usec / 1000);
#endif
}

extern "C" uint32_t to_ms (clock_t delta)
{
    return (uint32_t) ((float) delta / CLOCKS_PER_SEC * 1000);
}

uint64_t get_FILE_SIZE (std::ifstream *is)
{
    uint64_t f_size = 0;

    is->seekg(0, std::ios::end);
    f_size = (uint64_t) is->tellg();
    is->seekg(0, std::ios::beg);

    return f_size;
}

extern "C" const char *str_md5 (apr_pool_t * p, const char *pSTR, int32_t pSTR_len)
{
    /* Calculate the MD5 sum of the file */
    union {
        unsigned char chr[APR_MD5_DIGESTSIZE];
        uint32_t num[4];
    } digest;
    apr_md5_ctx_t md5;

    int32_t idx = 0;
    int32_t len = APR_MD5_DIGESTSIZE / 4;
    char hex_output[(APR_MD5_DIGESTSIZE * 2) + 1];
    char *szSTR = NULL;

    if (pSTR != NULL && pSTR_len > 0)
    {
        memset (digest.chr, 0, APR_MD5_DIGESTSIZE);
        apr_md5_init (&md5);
        apr_md5_update (&md5, (const unsigned char *) pSTR, pSTR_len);
        apr_md5_final (digest.chr, &md5);

        memset (hex_output, 0, (APR_MD5_DIGESTSIZE * 2) + 1);
        for (idx = 0; idx < len; ++idx)
        {
            sprintf (hex_output + (idx * 8), "%08x", digest.num[idx]);
        }

        szSTR = apr_pstrndup (p, hex_output, std::strlen (hex_output));
    }

    return szSTR;
}

extern "C" hx_shm_rec_t *hx_shm_attach (apr_pool_t *p, const char *pDB_FILE)
{
    hx_shm_rec_t *v_shm_t = nullptr;

    if (!pDB_FILE)
    {
        return nullptr;
    }

    key_t v_key = ftok (pDB_FILE, 'H');
    if (v_key != -1)
    {
        int32_t v_id = shmget (v_key, 0, 0);
        if (v_id == -1)
        {
            std::cout << "cannot attach to shared memory: " << strerror (errno) << std::endl;
            return nullptr;
        }
        else
        {
            struct shmid_ds shm_buf;
            memset((void *)&shm_buf, 0, sizeof(struct shmid_ds));
            shmctl(v_id, IPC_STAT, &shm_buf);
            uint64_t v_size = (uint64_t)shm_buf.shm_segsz;

            const void *v_ptr = (const void *) shmat (v_id, (void *) NULL, SHM_RDONLY);
            if (v_ptr == (const void *) (-1))
            {
                std::cout << "cannot attach to shared memory: " << strerror (errno) << std::endl;
                return nullptr;
            }
            else
            {
                v_shm_t = (hx_shm_rec_t *) apr_palloc (p, sizeof (hx_shm_rec_t));
                v_shm_t->v_key = v_key;
                v_shm_t->v_id = v_size;
                v_shm_t->v_size = v_size;
                v_shm_t->v_ptr = v_ptr;
            }
        }
    }

    return v_shm_t;
}

extern "C" void hx_shm_detach (hx_shm_rec_t *p_shm_t)
{
    if (p_shm_t && p_shm_t->v_ptr)
    {
        shmdt (p_shm_t->v_ptr);
    }
}
