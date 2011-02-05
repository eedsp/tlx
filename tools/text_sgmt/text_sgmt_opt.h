//
// Created by Shiwon Cho on 2005.10.28.
//

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <apr_general.h>
#include <apr_getopt.h>
#include <apr_file_io.h>
#include <jansson.h>

#include "tlx_types.h"

typedef struct _v_config_t
{
    int32_t         v_show_help;
    int32_t         v_test_conf;
    int32_t         v_debug;
    int32_t         v_shm_id;
    const char      *conf_filename;       // configuration filename
    const char      *db_path;             // db path
    const char      *db_phrase;           // db phrase
    std::vector<std::string> v_files;                   // file_list
    json_t  *v_json;
} v_config_t;

int32_t get_options (apr_pool_t *p_pool, int32_t argc, const char *argv[], v_config_t *pCFG);
int32_t load_config (apr_pool_t *p_pool, v_config_t *pCFG);

