//
// Created by Shiwon Cho on 2005.10.28.
//

#pragma once

#include <apr_general.h>
#include <apr_getopt.h>
#include <apr_file_io.h>
#include <vector>
#include <jansson.h>

#include <iostream>
#include <fstream>
#include <string>

#include "tlx_types.h"

typedef struct _v_config_t
{
    int             v_test_conf;
    int             v_debug;
    const char    *v_app;               // application name
    const char    *iPATH;               // input path
    const char    *oPATH;               // output path
    const char    *tPATH;               // temporary path
    const char    *tFILE;               // temporary file
    const char    *oFILE;               // output file
    const char    *conf_filename;       // configuration filename
    std::vector<std::string> v_files;   // file_list
    std::vector<std::string> v_str;     // string list
    json_t  *v_json;
} v_config_t;

int32_t get_options (apr_pool_t *p_pool, int32_t argc, const char *argv[], v_config_t *pCFG);
int32_t load_config (apr_pool_t *p_pool, v_config_t *pCFG);

