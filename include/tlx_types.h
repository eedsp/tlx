//
// Created by Shiwon Cho on 2005.10.06.
//

#pragma once

#include <apr_pools.h>
#include <apr_strings.h>

#	define _INFO(p)	        apr_psprintf ((p), "%5d: ", __LINE__)

//  Protocol signal constants
#define vSIGNAL_READY                           "\001"  //  Signals v_worker is ready
#define vSIGNAL_HEARTBEAT                       "\002"  //  Signals v_worker heartbeat

// name of configuration

#define vHX_FORMAT_JSON                         "json"
#define vHX_FORMAT_TEXT                         "text"
#define vHX_FORMAT_RAW                          "raw"
#define vHX_FORMAT_URLENCODE                    "urlencode"

#define vHX_WORKING_DIRECTIRY                   "working_directory"
#define vHX_PID_FILE                            "pid_file"

#define vHX_LOG_CONSOLE                         "console"
#define vHX_LOG_LEVEL                           "log_level"
#define vHX_LOG_FILE                            "log_file"

typedef int32_t hx_status_t; /* return type */
typedef int32_t hx_err_t;     /* error type */

typedef enum
{
    HX_OK       = 0,
    HX_ERROR    = -1,
    HX_EAGAIN   = -2,
    HX_ENOMEM   = -3
} _hx_status_t;

typedef enum {
    HX_FORMAT_NONE      = -1,
    HX_FORMAT_RAW       =  0,
    HX_FORMAT_JSON      =  1,
    HX_FORMAT_TEXT      =  2
} _hx_opt_out_format;

typedef enum {
    HX_APP_NONE         = -1,
    HX_APP_NORMLZR      =  0,
    HX_APP_TOKEN        =  1
} _hx_opt_api;

typedef enum
{
    _FORMAT_MSGPACK     = 1,
    _FORMAT_JSON        = 2,
    _FORMAT_RAW         = 3,
    _FORMAT_URLENCODE   = 4
} _hx_opt_data_format;

typedef enum
{
    active,
    update,
    paused,
    terminated,
} _proxy_state_t ;


