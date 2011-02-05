#include <iostream>
#include <cstdio>
#include <cstring>

#include "vx_type.h"
#include "vtx.h"

#define V_ENTRY_SIZE 20

vtx_list_t *vtx_create (apr_pool_t *p)
{
    vtx_list_t *p_vtx_l = (vtx_list_t *)apr_palloc (p, sizeof(vtx_list_t));

    if (p_vtx_l != NULL)
    {
        p_vtx_l->v_status = apr_pool_create (&p_vtx_l->p, p);
        p_vtx_l->v_size = 0;
        if (p_vtx_l->v_status == APR_SUCCESS && p_vtx_l->p != NULL)
        {
            p_vtx_l->v_l = (vx_list_t *)apr_palloc(p_vtx_l->p, sizeof(vx_list_t));
            if (p_vtx_l->v_l != NULL)
            {
                APR_RING_INIT (p_vtx_l->v_l, _vx_rec_t, vlink);
            }
        }
    }

    return p_vtx_l;
}

void vtx_clear (vtx_list_t *p_vtx_l)
{
    if (p_vtx_l == NULL) return;
    if (p_vtx_l->v_status == APR_SUCCESS && p_vtx_l->p != NULL)
    {
        apr_pool_clear (p_vtx_l->p);

        p_vtx_l->v_size = 0;
        p_vtx_l->v_l = (vx_list_t *)apr_palloc(p_vtx_l->p, sizeof(vx_list_t));

        if (p_vtx_l->v_l != NULL)
        {
            APR_RING_INIT (p_vtx_l->v_l, _vx_rec_t, vlink);
        }
    }
}

void vtx_delete (vtx_list_t *p_vtx_l)
{
    if (p_vtx_l == NULL) return;
    if (p_vtx_l->v_status == APR_SUCCESS && p_vtx_l->p != NULL)
    {
        apr_pool_destroy (p_vtx_l->p);
        p_vtx_l->p = NULL;
        p_vtx_l->v_size = 0;
        p_vtx_l->v_l  = NULL;
    }
}

void vtx_push_back (vtx_list_t *p_vtx_l, void *p_elem)
{
    if (p_vtx_l == NULL || p_elem == NULL)
    {
        return;
    }
    vx_rec_t *vx_ptr = (vx_rec_t *)apr_palloc (p_vtx_l->p, sizeof (vx_rec_t));

    if (vx_ptr != NULL)
    {
        vx_ptr->v = (void *)p_elem;
        APR_RING_INSERT_TAIL (p_vtx_l->v_l, vx_ptr, _vx_rec_t, vlink);
        p_vtx_l->v_size++;
    }
}

void vtx_insert (vtx_list_t *p_vtx_l, void *p_elem)
{
    if (p_vtx_l == NULL || p_elem == NULL) return;
    vx_rec_t *vx_ptr = (vx_rec_t *)apr_palloc (p_vtx_l->p, sizeof (vx_rec_t));

    if (vx_ptr != NULL)
    {
        vx_ptr->v = (void *)p_elem;
        APR_RING_INSERT_TAIL (p_vtx_l->v_l, vx_ptr, _vx_rec_t, vlink);
        p_vtx_l->v_size++;
    }
}

void vtx_insert_head (vtx_list_t *p_vtx_l, void *p_elem)
{
    if (p_vtx_l == NULL || p_elem == NULL) return;
    vx_rec_t *vx_ptr = (vx_rec_t *)apr_palloc (p_vtx_l->p, sizeof (vx_rec_t));

    if (vx_ptr != NULL)
    {
        vx_ptr->v = (void *)p_elem;
        APR_RING_INSERT_HEAD (p_vtx_l->v_l, vx_ptr, _vx_rec_t, vlink);
        p_vtx_l->v_size++;
    }
}

void vtx_push_back (vtx_list_t *p_vtx_l, int32_t pIDX,
                    int32_t p_idx_sgmt,
                    int32_t p_idx_elt,
                    const char *pSTR,
                    size_t pSTR_len,
                    int32_t ucs_offset, int32_t ucs_len,
                    int32_t utf8_offset, int32_t utf8_len,
                    const char *pTAG
)
{
	vtx_rec_t *p_rec = (vtx_rec_t *)apr_palloc (p_vtx_l->p, sizeof (vtx_rec_t));

	if (p_rec != NULL)
	{
		p_rec->v_idx_t		       = pIDX;
		p_rec->v_idx_sgmt		   = p_idx_sgmt;
		p_rec->v_idx_elt		   = p_idx_elt;
		p_rec->ptr                 = pSTR;
		p_rec->ptr_len             = pSTR_len;
		p_rec->v_tag               = pTAG;
		p_rec->ucs_offset          = ucs_offset;
		p_rec->ucs_len             = ucs_len;
		p_rec->utf8_offset         = utf8_offset;
		p_rec->utf8_len            = utf8_len;

		vtx_push_back (p_vtx_l, (void *)p_rec);
	}
}

void vtx_print (const vtx_list_t *p_vtx_l)
{
    if (p_vtx_l != NULL && !APR_RING_EMPTY (p_vtx_l->v_l, _vx_rec_t, vlink))
    {
        const vx_rec_t *p_vx    = NULL;
        APR_RING_FOREACH (p_vx, p_vtx_l->v_l, _vx_rec_t, vlink)
        {
        	const vtx_rec_t *p_rec = (vtx_rec_t *)p_vx->v;

            std::cout << apr_psprintf(p_vtx_l->p, "%4d [%3d|%2d] %5d(%3d) %5d(%3d)",
                    p_rec->v_idx_t,
                    p_rec->v_idx_sgmt,
                    p_rec->v_idx_elt,
                    (int32_t) p_rec->ucs_offset, (int32_t) p_rec->ucs_len,
                    (int32_t) p_rec->utf8_offset, (int32_t) p_rec->utf8_len
                );
                std::cout << apr_psprintf(p_vtx_l->p, " [%*s]", V_ENTRY_SIZE, p_rec->v_tag);
                std::cout << apr_psprintf(p_vtx_l->p, " [%.*s]", (int32_t)p_rec->ptr_len, p_rec->ptr);
                std::cout << std::endl;
        }
    }
}

const char *vtx_text_print (const vtx_list_t *p_vtx_l)
{
    const char *v_buffer = nullptr;

    if (p_vtx_l != NULL && !APR_RING_EMPTY (p_vtx_l->v_l, _vx_rec_t, vlink))
    {
        struct iovec *v_vec = (struct iovec *)apr_palloc(p_vtx_l->p, sizeof(struct iovec) *p_vtx_l->v_size);

        int32_t v_idx = 0;
        const vx_rec_t *p_vx    = nullptr;
        APR_RING_FOREACH (p_vx, p_vtx_l->v_l, _vx_rec_t, vlink)
        {
        	const vtx_rec_t *p_rec = (vtx_rec_t *)p_vx->v;

            const char *szSTR = apr_psprintf(p_vtx_l->p, "%d\t%d\t%d\t%d\t%d\t%.*s\t%s\n",
                p_rec->v_idx_t,
                (int32_t) p_rec->ucs_offset, (int32_t) p_rec->ucs_len,
                (int32_t) p_rec->utf8_offset, (int32_t) p_rec->utf8_len,
                (int32_t) p_rec->ptr_len, p_rec->ptr,
                p_rec->v_tag
            );
            v_vec[v_idx].iov_base = (void *)szSTR;
            v_vec[v_idx].iov_len = (size_t)std::strlen(szSTR);
            v_idx++;
        }
        v_buffer = apr_pstrcatv (p_vtx_l->p, v_vec, v_idx, NULL);
    }

    return v_buffer;
}

#if 1
const char * vtx_json_print (const vtx_list_t *p_vtx_l)
{
    const char *v_buffer = nullptr;
    json_t *v_token_list = json_array();

    if (v_token_list && p_vtx_l != NULL && !APR_RING_EMPTY (p_vtx_l->v_l, _vx_rec_t, vlink))
    {
        const vx_rec_t *p_vx = NULL;
        APR_RING_FOREACH (p_vx, p_vtx_l->v_l, _vx_rec_t, vlink)
        {
            const vtx_rec_t *p_rec = (vtx_rec_t *) p_vx->v;
            json_t *v_token_rec = json_object();

            json_object_set_new (v_token_rec, "idx", json_integer((json_int_t)p_rec->v_idx_t));
            json_object_set_new (v_token_rec, "ucs_offset", json_integer((json_int_t)p_rec->ucs_offset));
            json_object_set_new (v_token_rec, "ucs_length", json_integer((json_int_t)p_rec->ucs_len));
            json_object_set_new (v_token_rec, "utf8_offset", json_integer((json_int_t)p_rec->utf8_offset));
            json_object_set_new (v_token_rec, "uts8_length", json_integer((json_int_t)p_rec->utf8_len));
            json_object_set_new (v_token_rec, "token_str", json_stringn((const char *)p_rec->ptr, p_rec->ptr_len));
            json_object_set_new (v_token_rec, "token_tag", json_string((const char *)p_rec->v_tag));

            if (v_token_rec)
            {
                json_array_append_new(v_token_list, v_token_rec);
            }
        }
        int32_t v_flags = JSON_INDENT(4) | JSON_ENCODE_ANY | JSON_ESCAPE_SLASH; // default 0
        const char *v_tmp = (const char *) json_dumps(v_token_list, v_flags);
        if (v_tmp)
        {
            v_buffer = apr_pstrdup (p_vtx_l->p, v_tmp);
            free((void *) v_tmp);
        }
        json_decref(v_token_list);
    }

    return v_buffer;
}
#else
const char * vtx_json_print (const vtx_list_t *p_vtx_l)
{
    const char *v_buffer = NULL;
    json_t *v_token = json_object();

    if (v_token && p_vtx_l != NULL && !APR_RING_EMPTY (p_vtx_l->vx_l, _vx_rec_t, vlink))
    {
        json_t *v_token_list = json_array();
        if (v_token_list)
        {
            const vx_rec_t *p_vx = NULL;
            APR_RING_FOREACH (p_vx, p_vtx_l->vx_l, _vx_rec_t, vlink)
            {
                const vtx_rec_t *p_rec = (vtx_rec_t *) p_vx->v;
#if 1
                json_t *v_token_rec = json_object();

                json_object_set_new (v_token_rec, "idx", json_integer((json_int_t)p_rec->v_idx_t));
                json_object_set_new (v_token_rec, "ucs.offset", json_integer((json_int_t)p_rec->ucs_offset));
                json_object_set_new (v_token_rec, "ucs.length", json_integer((json_int_t)p_rec->ucs_len));
                json_object_set_new (v_token_rec, "utf8.offset", json_integer((json_int_t)p_rec->utf8_offset));
                json_object_set_new (v_token_rec, "uts8.length", json_integer((json_int_t)p_rec->utf8_len));
                json_object_set_new (v_token_rec, "token.str", json_stringn((const char *)p_rec->ptr, p_rec->ptr_len));
                json_object_set_new (v_token_rec, "token.tag", json_string((const char *)p_rec->v_tag));
#else
                json_t *v_token_rec = json_pack("{s:i, s:i, s:i, s:i, s:i, s:s, s:s}",
                    "idx", p_rec->v_idx_t,
                    "ucs.offset", p_rec->ucs_offset,
                    "ucs.length", p_rec->ucs_len,
                    "utf8.offset", p_rec->utf8_offset,
                    "uts8.length", p_rec->utf8_len,
                    "token.str", apr_pstrndup(p_vtx_l->p, (char *) p_rec->ptr, (int32_t) p_rec->ptr_len),
                    "token.tag", p_rec->v_tag
                );
#endif

                if (v_token_rec)
                {
                    json_array_append_new(v_token_list, v_token_rec);
                }
            }
            if (json_array_size(v_token_list) > 0)
            {
                json_object_set_new(v_token, "_token", v_token_list);
            }
        }
        int32_t v_flags = JSON_INDENT(2) | JSON_COMPACT | JSON_ENCODE_ANY | JSON_ESCAPE_SLASH; // default 0
        const char *v_tmp = (const char *) json_dumps(v_token, v_flags);
        if (v_tmp)
        {
            v_buffer = apr_pstrdup (p_vtx_l->p, v_tmp);
            free((void *) v_tmp);
        }
        json_decref(v_token);
    }

    return v_buffer;
}
#endif
