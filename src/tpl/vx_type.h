#pragma once

#include <apr_ring.h>

#if 0

#define _STR_INDENT_            "  "

#define CHECK_VALUE(a, b)     ((a) == 0 || ((a) > 0 && (b) < (a)))

#define array_size(a)  ( sizeof(a) / sizeof(*(a)) )

#define apr_idx(p, idx)             APR_ARRAY_IDX(p, idx, const vl_ptr_rec_t*)
#define apr_array_char(p, idx)      APR_ARRAY_IDX((p), (idx), const char*)
#define apr_array_vq_rec(p, idx)    APR_ARRAY_IDX(p, idx, const vq_rec*)
#define apr_array_tag(p, idx)       APR_ARRAY_IDX(p, idx, const tag_rec_t*)
#define apr_array_dic(p, idx)       APR_ARRAY_IDX(p, idx, const dic_rec_t*)
#define apr_array_pv_rec(p, idx)    APR_ARRAY_IDX(p, idx, const pv_rec_t*)
#define apr_array_vl_ptr(p, idx)    APR_ARRAY_IDX(p, idx, const vl_ptr_rec_t*)

#define HASH_GET(ht, pKEY)          apr_hash_get ((ht), (const void *)pKEY, APR_HASH_KEY_STRING)
#define HASH_SET(ht, pKEY, pVAL)    apr_hash_set ((ht), (const void *)(pKEY), APR_HASH_KEY_STRING, (const void *)(pVAL))
#define HASH_SIZE(ht)               apr_hash_count ((ht))

#endif

typedef struct _vx_rec_t
{
    APR_RING_ENTRY (_vx_rec_t) vlink;

    void *v;
} vx_rec_t;

/* Ring container type */
typedef struct _vx_list_t vx_list_t;

APR_RING_HEAD (_vx_list_t, _vx_rec_t);

typedef struct _vtx_list_t
{
    apr_pool_t *p;
    apr_status_t v_status;

    vx_list_t *v_l;

    int32_t v_size;
} vtx_list_t;

typedef struct _vtx_rec_t
{
    int32_t v_idx_t;        // index of token
    int32_t v_idx_sgmt;        // index of token segment
    int32_t v_idx_elt;      // index of token element

    const char *ptr;        // string pointer
    size_t ptr_len;        // string length
    const char *v_tag;        // pos tag

    int32_t ucs_offset;        // unicode offset
    int32_t ucs_len;        // unicode length
    int32_t utf8_offset;    // utf-8 offset
    int32_t utf8_len;        // utf-8 length
} vtx_rec_t;

