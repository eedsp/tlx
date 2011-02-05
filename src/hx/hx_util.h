//
// Created by Shiwon Cho on 2002.08.24.
//

#pragma once

#include <cctype>
#include <ctime>

#include <stdarg.h>
#include <apr_general.h>

#define LF                  (uint8_t) 10
#define CR                  (uint8_t) 13
#define CRLF                "\x0d\x0a"
#define CRLF_LEN            (sizeof("\x0d\x0a") - 1)

#define NELEMS(a)           ((sizeof(a)) / sizeof((a)[0]))

#ifndef MIN
#define MIN(a, b)           ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b)           ((a) > (b) ? (a) : (b))
#endif

#define SQUARE(d)           ((d) * (d))
#define VAR(s, s2, n)       (((n) < 2) ? 0.0 : ((s2) - SQUARE(s)/(n)) / ((n) - 1))
#define STDDEV(s, s2, n)    (((n) < 2) ? 0.0 : sqrt(VAR((s), (s2), (n))))

#define HX_INET4_ADDRSTRLEN (sizeof("255.255.255.255") - 1)
#define HX_INET6_ADDRSTRLEN \
    (sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255") - 1)
#define HX_INET_ADDRSTRLEN  MAX(HX_INET4_ADDRSTRLEN, HX_INET6_ADDRSTRLEN)
#define HX_UNIX_ADDRSTRLEN  \
    (sizeof(struct sockaddr_un) - offsetof(struct sockaddr_un, sun_path))

#define HX_MAXHOSTNAMELEN   256

/*
 * Length of 1 byte, 2 bytes, 4 bytes, 8 bytes and largest integral
 * type (uintmax_t) in ascii, including the null terminator '\0'
 *
 * From stdint.h, we have:
 * # define UINT8_MAX   (255)
 * # define UINT16_MAX  (65535)
 * # define UINT32_MAX  (4294967295U)
 * # define UINT64_MAX  (__UINT64_C(18446744073709551615))
 */
#define HX_UINT8_MAXLEN     (3 + 1)
#define HX_UINT16_MAXLEN    (5 + 1)
#define HX_UINT32_MAXLEN    (10 + 1)
#define HX_UINT64_MAXLEN    (20 + 1)
#define HX_UINTMAX_MAXLEN   HX_UINT64_MAXLEN

/*
 * Make data 'd' or pointer 'p', n-byte aligned, where n is a power of 2
 * of 2.
 */
#define HX_ALIGNMENT        sizeof(unsigned long) /* platform word */
#define HX_ALIGN(d, n)      (((d) + (n - 1)) & ~(n - 1))
#define HX_ALIGN_PTR(p, n)  \
    (void *) (((uintptr_t) (p) + ((uintptr_t) n - 1)) & ~((uintptr_t) n - 1))

/*
 * Wrapper to workaround well known, safe, implicit type conversion when
 * invoking system calls.
 */
#define HX_gethostname(_name, _len) \
    gethostname((char *)_name, (size_t)_len)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _hx_shm_rec_t
{
    const void  *v_ptr;     // shared memory ptr
    key_t       v_key;  // shared memory key
    uint64_t    v_size;     // shared memory size
    int32_t     v_id;
} hx_shm_rec_t;

int64_t s_clock ();
const char *str_md5 (apr_pool_t * p_pool, const char *pSTR, int32_t pSTR_len);
hx_shm_rec_t *hx_shm_attach (apr_pool_t *p, const char *pDB_FILE);
void hx_shm_detach (hx_shm_rec_t *p_shm_t);

#ifdef __cplusplus
}
#endif

