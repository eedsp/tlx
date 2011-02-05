#include <iostream>
#include <sstream>

#include <sys/shm.h>
#include <apr_pools.h>
#include <apr_shm.h>
#include <apr_strings.h>

#include "db2shm_opt.h"

#define vAPP_NAME           "db2shm"

static const void *proc_file_read (apr_pool_t *p_pool, const char *pFILENAME, apr_size_t &v_FILESIZE)
{
    apr_file_t *v_fp = NULL;
    apr_status_t v_status = -1;
    apr_finfo_t v_finfo;
    const void *szDATA = NULL;

    v_FILESIZE = 0;

    if ((v_status = apr_file_open(&v_fp, pFILENAME, APR_READ | APR_BUFFERED | APR_BINARY, APR_OS_DEFAULT, p_pool)) == APR_SUCCESS)
    {
        apr_file_info_get(&v_finfo, APR_FINFO_NORM, v_fp);

        apr_size_t v_fsize = 0;
        szDATA = (const void *) apr_palloc(p_pool, (apr_size_t) v_finfo.size + 1);

        apr_file_read_full(v_fp, (void *) szDATA, v_finfo.size, &v_fsize);

        v_FILESIZE = v_fsize;

        apr_file_close(v_fp);
    }

    return szDATA;
}

int main (int32_t argc, const char *argv[])
{
    try
    {
        pid_t vPID = getpid();
        size_t log_queue_size = 1048576; //queue size must be power of 2
        spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e [%L] %v");
        spdlog::set_async_mode(log_queue_size);
        auto console = spdlog::stdout_logger_mt("console");

        apr_status_t v_apr_status = apr_initialize();
        apr_pool_t *v_pool = NULL;

        if (v_apr_status == APR_SUCCESS)
        {
            apr_pool_create(&v_pool, NULL);

            v_config_t vCFG;
            hx_status_t v_hx_status = get_options(v_pool, argc, argv, &vCFG);

            if (v_hx_status == HX_OK)
            {
                if (vCFG.v_file[0] != '#')
                {
                    key_t v_shm_key = -1;
                    const char *vFILE = NULL;

                    if (vCFG.v_path != NULL && vCFG.v_file != NULL)
                    {
                        vFILE = apr_psprintf (v_pool, "%s/%s", vCFG.v_path, vCFG.v_file);
                    }
                    else
                    {
                        console->error ("[{:d}] No such file or directory", vPID);
                        exit (-1);
                    }

                    if (vCFG.v_shm_key > 0)
                    {
                        v_shm_key = vCFG.v_shm_key;
                    }
                    else if (vFILE != NULL)
                    {
                        v_shm_key = ftok (vFILE, 'H');
                    }
                    console->info ("[{:d}] START.{}", vPID, vAPP_NAME);
                    console->info ("[{:d}] Shared Memory ID: {}", vPID, v_shm_key);

                    if (!apr_strnatcmp (vCFG.v_cmd, "import") && vFILE != NULL)
                    {
                        const void *pDATA = (const void *) proc_file_read (v_pool, vFILE, vCFG.v_file_size);

                        if (pDATA != NULL && vCFG.v_file_size > 0)
                        {
                            int32_t v_shm_id = shmget (v_shm_key, vCFG.v_file_size, SHM_R | SHM_W | IPC_CREAT); // 0644

                            void *v_ptr = shmat (v_shm_id, (void *) NULL, 0);
                            if (v_ptr != (char *) (-1))
                            {
                                memcpy (v_ptr, pDATA, vCFG.v_file_size);
                                shmdt (v_ptr);
                                console->info ("[{:d}] imported", vPID);
                            }
                            else
                            {
                                switch (errno)
                                {
                                    case EACCES:
                                        console->error ("[{:d}] linking to shared memory segment (for import): Access denied", vPID);
                                        break;
                                    case ENOENT:
                                        console->error ("[{:d}] linking to shared memory segment (for import): Segment does not exist", vPID);
                                        break;
                                    default:
                                        console->error ("[{:d}] linking to shared memory segment (for import) failed", vPID);
                                        break;
                                }
                            }
                        }
                        else
                        {
                            console->error ("[{:d}] cannot read file: {}/{}", vPID, vCFG.v_path, vCFG.v_file);
                        }

                    }
                    else if (!apr_strnatcmp (vCFG.v_cmd, "view"))
                    {
                        //                    int32_t v_shm_id = shmget (v_shm_key, 0, 0);
                        int32_t v_shm_id = shmget (v_shm_key, 0, SHM_RDONLY);

                        const void *v_ptr = (char *) shmat (v_shm_id, (void *) NULL, 0);
                        if (v_ptr != (char *) (-1))
                        {
                            const char *p_str = (const char *) v_ptr;
                            console->info ("[{:d}] [{}]", vPID, p_str);
                        }
                        else
                        {
                            switch (errno)
                            {
                                case EACCES:
                                    console->error ("[{:d}] cannot attach to shared memory (No permission)", vPID);
                                    break;
                                default:
                                    console->error ("[{:d}] attaching shared memory segment failed", vPID);
                                    break;
                            }
                        }
                        shmdt ((const void *) v_ptr);
                    }
                    else if (!apr_strnatcmp (vCFG.v_cmd, "free"))
                    {
                        // Link to existing segment
                        int32_t v_shm_id = shmget (v_shm_key, 0, 0);
                        if (v_shm_id < 0)
                        {
                            switch (errno)
                            {
                                case EACCES:
                                    console->error ("[{:d}] linking to shared memory segment (for freeing): Access denied", vPID);
                                    break;
                                case ENOENT:
                                    console->error ("[{:d}] linking to shared memory segment (for freeing): Segment does not exist", vPID);
                                    break;
                                default:
                                    console->error ("[{:d}] linking to shared memory segment (for freeing) failed", vPID);
                                    break;
                            }
                        }
                        else
                        {
                            // Free the segment
                            int32_t v_ret = shmctl (v_shm_id, IPC_RMID, NULL);
                            if (v_ret == -1)
                            {
                                switch (errno)
                                {
                                    case EPERM:
                                        console->error ("[{:d}] freeing shared memory segment: Permission denied", vPID);
                                        break;
                                    default:
                                        console->error ("[{:d}] freeing shared memory segment failed", vPID);
                                        break;
                                }
                            }
                            else
                            {
                                console->info ("[{:d}] remove shared memory segment", vPID);
                            }
                        }
                    }

                    console->info ("[{:d}] END.{}", vPID, vAPP_NAME);
                }
            }

            apr_pool_destroy(v_pool);
        }

        apr_terminate();



        spdlog::drop_all();
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log failed: " << ex.what() << std::endl;
    }
    return 0;
}

