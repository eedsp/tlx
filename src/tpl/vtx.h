#pragma once

#include <cstdlib>
#include <cstdio>
#include <cctype>

#include <apr_pools.h>
#include <apr_strings.h>
#include <apr_ring.h>

#include "vx_type.h"
#include <jansson.h>

vtx_list_t *vtx_create (apr_pool_t *p);
void vtx_clear (vtx_list_t *p_vtx);
void vtx_delete (vtx_list_t *p_vtx);
void vtx_push_back (vtx_list_t *p_vtx, void *p_elem);
void vtx_insert (vtx_list_t *p_vtx, void *p_elem);
void vtx_insert_head (vtx_list_t *p_vtx, void *p_elem);
void vtx_push_back (vtx_list_t *p_vtx, int32_t pIDX,
                    int32_t p_idx_sgmt,
                    int32_t p_idx_elt,
                    const char *pSTR,
                    size_t pSTR_len,
                    int32_t ucs_offset, int32_t ucs_len,
                    int32_t utf8_offset, int32_t utf8_len,
                    const char *pTAG
);

void vtx_print (const vtx_list_t *p_vtx_l);
const char *vtx_text_print (const vtx_list_t *p_vtx_l);
const char *vtx_json_print (const vtx_list_t *p_vtx_l);

