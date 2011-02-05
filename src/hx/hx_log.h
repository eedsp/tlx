//
// Created by Shiwon Cho on 2006.07.17.
//

#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include <apr_general.h>
#include <apr_getopt.h>
#include <apr_pools.h>
#include <apr_file_io.h>
#include <spdlog/spdlog.h>

#include "tlx_types.h"

class hx_log
{
  private:
    std::shared_ptr<spdlog::logger> v_daily_log_file;
    std::shared_ptr<spdlog::logger> v_console;

    struct custom_daily_file_name
    {
        static spdlog::filename_t calc_filename(const spdlog::filename_t& basename)
        {
            std::tm tm = spdlog::details::os::localtime();
            std::conditional<std::is_same<spdlog::filename_t::value_type, char>::value, fmt::MemoryWriter, fmt::WMemoryWriter>::type w;
            w.write(SPDLOG_FILENAME_T("{}.{:04d}{:02d}{:02d}.{}"), basename, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, "log");
            return w.str();
        }
    };

#if 0
    struct custom_daily_file_name
    {
        // Create filename for the form basename.YYYYMMDD.extension
        static spdlog::filename_t calc_filename(const spdlog::filename_t& basename, const spdlog::filename_t& extension)
        {
            std::tm tm = spdlog::details::os::localtime();
            std::conditional<std::is_same<spdlog::filename_t::value_type, char>::value, fmt::MemoryWriter, fmt::WMemoryWriter>::type w;
            w.write(SPDLOG_FILENAME_T("{}.{:04d}{:02d}{:02d}.{}"), basename, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, extension);
//            w.write(SPDLOG_FILENAME_T("{}.{:04d}{:02d}{:02d}_{:02d}{:02d}{:02d}.{}"), basename,
//                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
//                tm.tm_hour, tm.tm_min, tm.tm_sec,
//                extension);

            return w.str();
        }
    };
#endif

  public:
    hx_log();
    ~hx_log();

    void daily_logger (const char *pNAME, const char *pBASE, const char *pEXT = nullptr);
    void rotating_logger (const char *pNAME, const char *pBASE, const char *pEXT = nullptr, int32_t pMAX_FILES = 5);
    void set_level (spdlog::level::level_enum pLEVEL);
    spdlog::level::level_enum get_level (const char *pLogLevel);
};

