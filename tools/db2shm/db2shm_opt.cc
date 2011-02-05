#include <apr_general.h>
#include <apr_file_io.h>

#include "db2shm_opt.h"

static void set_default_options (v_config_t *pCFG)
{
    pCFG->_debug        = false;
    pCFG->v_cmd         = NULL;
    pCFG->v_path        = NULL;
    pCFG->v_file        = NULL;
    pCFG->v_shm_key     = -1;
    pCFG->v_file_size = 0;
}

static void show_usage (const char *pARGV)
{
    fprintf(stderr, "Usage: %s [-h] [-c command] [-d file_path] [ -f file_name]\n", pARGV);
    fprintf(stderr,
            "Options:\n"
            "  -h, --help          : this help\n"
            "  -c, --cmd           : command\n"
            "  -m, --shmid         : shmID\n"
            "  -d, --dir           : file path\n"
            "  -f, --file FILENAME : file name\n");
}

int32_t get_options (apr_pool_t *p_pool, int32_t argc, const char *argv[], v_config_t *pCFG)
{
    static const apr_getopt_option_t v_cmd_options[] = {
        /* long-option, short-option, has-arg flag, description */
        {"cmd",  'c', TRUE, "command"},    /* -c command or --cmd command */
        {"dir", 'd', TRUE, "file path"},    /* -d path or --dir path */
        {"file", 'f', TRUE, "file name"},    /* -f name or --file name */
        {"smhid", 'm', TRUE, "shmID"},    /* -m shmID or --shmid shmID */
        {"help", 'h', FALSE, "show help"},    /* -h or --help */
        {NULL, 0, 0, NULL}, /* end */
    };

    auto console = spdlog::get("console");
    apr_getopt_t *v_opt = NULL;
    int32_t optch = 0;
    const char *optarg = NULL;
    apr_status_t v_opt_status = APR_SUCCESS;

    set_default_options (pCFG);
    /* initialize apr_getopt_t */
    apr_getopt_init(&v_opt, p_pool, argc, argv);

    /* parse the all options based on opt_option[] */
    while ((v_opt_status = apr_getopt_long(v_opt, v_cmd_options, &optch, &optarg)) == APR_SUCCESS)
    {
        switch (optch)
        {
            case 'c':
                pCFG->v_cmd = (char *) optarg;
                break;
            case 'm':
                pCFG->v_shm_key = (key_t) apr_atoi64 (optarg);
                break;
            case 'd':
                pCFG->v_path = (char *) optarg;
                break;
            case 'f':
                pCFG->v_file = (char *) optarg;
                break;
            case 'h':
                show_usage((const char *) argv[0]);
                return HX_ERROR;
            default:
                fprintf(stderr, "%s: invalid option -- '%c'\n", argv[0], optch);
                show_usage((const char *) argv[0]);
                return HX_ERROR;
        }
    }

    return HX_OK;
}


