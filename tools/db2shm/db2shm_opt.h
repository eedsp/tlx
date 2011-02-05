#pragma once

#include <sys/ipc.h>
#include <sys/shm.h>

#include <apr_general.h>
#include <apr_getopt.h>
#include <apr_file_io.h>

#include <iostream>
#include <fstream>
#include <string>
#include <spdlog/spdlog.h>
#include "tlx_types.h"

typedef struct _v_config_t
{
    bool        _debug;
    const char  *v_path;        // dictionary path
    const char  *v_file;        // dictionary filename
    const char  *v_cmd;
    key_t       v_shm_key;
    apr_size_t  v_file_size;

} v_config_t;

int32_t get_options (apr_pool_t *p_pool, int32_t argc, const char *argv[], v_config_t *pCFG);

