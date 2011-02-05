#include "t_opt.h"

static void set_default_options (v_config_t *pCFG)
{
    pCFG->v_debug = 0;
    pCFG->v_test_conf = 0;
    pCFG->v_app = NULL;
    pCFG->iPATH = NULL;
    pCFG->oPATH = NULL;
    pCFG->tPATH = NULL;
    pCFG->tFILE = NULL;
    pCFG->oFILE = NULL;
    pCFG->conf_filename = NULL;
    pCFG->v_files.clear ();
    pCFG->v_str.clear ();
    pCFG->v_json = NULL;
}

static void show_usage (const char *pARGV)
{
    fprintf (stderr, "Usage: %s [-ht] [-c config_file] [file ...]\n", pARGV);
    fprintf (stderr,
        "Options:\n"
            " -h, --help          : this help\n"
            " -d, --debug         : Debug mode\n"
            " -t, --test          : test configuration for syntax errors and exit\n"
            " -c, --conf FILENAME : set configuration file\n");
}

int32_t get_options (apr_pool_t *p_pool, int32_t argc, const char *argv[], v_config_t *pCFG)
{
    static const apr_getopt_option_t v_cmd_options[] = {
        /* long-option, short-option, has-arg flag, description */
        {"test", 't', FALSE, "test configuration for syntax errors and exit"},      /* -t or --test */
        {"debug", 'd', FALSE, "Debug mode"},      /* -d or --debug */
        {"conf", 'c', TRUE, "set configuration file"},    /* -c name or --config name */
        {"help", 'h', FALSE, "show help"},    /* -h or --help */
        {NULL, 0, 0, NULL}, /* end */
    };

    apr_getopt_t *v_opt = NULL;
    int optch = 0;
    const char *optarg = NULL;
    apr_status_t v_opt_status = APR_SUCCESS;

    set_default_options (pCFG);
    /* initialize apr_getopt_t */
    apr_getopt_init (&v_opt, p_pool, argc, argv);

    /* parse the all options based on opt_option[] */
    while ((v_opt_status = apr_getopt_long (v_opt, v_cmd_options, &optch, &optarg)) == APR_SUCCESS)
    {
        switch (optch)
        {
            case 't':
                pCFG->v_test_conf = 1;
                break;
            case 'd':
                pCFG->v_debug = 1;
                break;
            case 'c':
                pCFG->conf_filename = (char *) optarg;
                break;
            case 'h':
                show_usage ((const char *) argv[0]);
                return HX_ERROR;
            default:
                fprintf (stderr, "%s: invalid option -- '%c'\n", argv[0], optch);
                show_usage ((const char *) argv[0]);
                return HX_ERROR;
        }
    }

    if (v_opt->ind < argc)
    {
        pCFG->v_files.clear ();
        for (int32_t v_idx = (int32_t) v_opt->ind; v_idx < argc; v_idx++)
        {
            pCFG->v_files.push_back (std::string (argv[v_idx]));
        }
    }

    return HX_OK;
}

int32_t load_config (apr_pool_t *p_pool, v_config_t *pCFG)
{
    std::FILE *v_fp = std::fopen (pCFG->conf_filename, "rb");

    if (v_fp)
    {
        json_error_t error;
        pCFG->v_json = json_loadf (v_fp, 0, &error);
        std::fclose (v_fp);

        if (pCFG->v_json && pCFG->conf_filename != NULL)
        {
            if (pCFG->v_test_conf)
            {
                std::cout << _INFO (p_pool) << apr_psprintf (p_pool, "configuration file '%s' syntax is OK\n", pCFG->conf_filename);
            }
            else
            {
                const char *v_key = NULL;
                json_t *v_obj = NULL;
                int32_t v_idx = 0;
                json_object_foreach(pCFG->v_json, v_key, v_obj)
                {
                    /* block of code that uses key and value */
                    if (json_is_string(v_obj))
                    {
                        const char *v_val = json_string_value (v_obj);

                        if (!apr_strnatcmp ("app", v_key))
                        {
                            pCFG->v_app = v_val;
                            if (pCFG->v_debug)
                            {
                                //                                std::cout << _INFO (p_pool) << apr_psprintf(p_pool, "[%3d]: %*s: %s\n", v_idx++, 15, v_key, pCFG->v_app);
                            }
                        }
                        else if (!apr_strnatcmp ("raw", v_key))
                        {
                            pCFG->tFILE = v_val;
                            if (pCFG->v_debug)
                            {
                                std::cout << _INFO (p_pool) << apr_psprintf (p_pool, "[%3d]: %*s: %s\n", v_idx++, 15, v_key, pCFG->tFILE);
                            }
                        }
                        else if (!apr_strnatcmp ("dic", v_key))
                        {
                            pCFG->oFILE = v_val;
                            if (pCFG->v_debug)
                            {
                                std::cout << _INFO (p_pool) << apr_psprintf (p_pool, "[%3d]: %*s: %s\n", v_idx++, 15, v_key, pCFG->oFILE);
                            }
                        }
                        else if (!apr_strnatcmp ("i_path", v_key))
                        {
                            pCFG->iPATH = v_val;
                            if (pCFG->v_debug)
                            {
                                std::cout << _INFO (p_pool) << apr_psprintf (p_pool, "[%3d]: %*s: %s\n", v_idx++, 15, v_key, pCFG->iPATH);
                            }
                        }
                        else if (!apr_strnatcmp ("o_path", v_key))
                        {
                            pCFG->oPATH = v_val;
                            if (pCFG->v_debug)
                            {
                                std::cout << _INFO (p_pool) << apr_psprintf (p_pool, "[%3d]: %*s: %s\n", v_idx++, 15, v_key, pCFG->oPATH);
                            }
                        }
                        else if (!apr_strnatcmp ("t_path", v_key))
                        {
                            pCFG->tPATH = v_val;
                            if (pCFG->v_debug)
                            {
                                std::cout << _INFO (p_pool) << apr_psprintf (p_pool, "[%3d]: %*s: %s\n", v_idx++, 15, v_key, pCFG->tPATH);
                            }
                        }
                    }
                } // json_object_foreach
            } // if
        }
        else
        {
            std::cout << _INFO (p_pool) << apr_psprintf (p_pool, "configuration file '%s' syntax is invalid\n", pCFG->conf_filename);
            std::cout << _INFO (p_pool)
                      << apr_psprintf (p_pool, "ERROR: %d %d %d\nERROR: %s\n", error.line, error.column, error.position, error.text);
            return HX_ERROR;
        }
    }
    else
    {
        std::cout << _INFO (p_pool) << apr_psprintf (p_pool, "configuration file '%s' not found\n", pCFG->conf_filename);
        return HX_ERROR;
    }

    return HX_OK;
}
