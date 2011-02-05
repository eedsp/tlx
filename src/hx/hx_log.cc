//
// Created by Shiwon Cho on 2006.07.17.
//

#include "hx_log.h"

hx_log::hx_log ()
{
    try
    {
//        size_t log_queue_size = 1048576; //queue size must be power of 2: 2^20
//        size_t log_queue_size = 64; //queue size must be power of 2: 2^20
        size_t log_queue_size = 524288; //queue size must be power of 2: 2^19
        spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e [%L] %v");

        auto flush_interval_ms = std::chrono::milliseconds::zero();
        spdlog::set_async_mode(log_queue_size, spdlog::async_overflow_policy::block_retry, nullptr, flush_interval_ms, nullptr);
//        spdlog::set_async_mode(log_queue_size, spdlog::async_overflow_policy::block_retry);
        v_console = spdlog::stdout_logger_mt(vHX_LOG_CONSOLE);
        v_daily_log_file = nullptr;
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log failed: " << ex.what() << std::endl;
    }
}

void hx_log::daily_logger (const char *pNAME, const char *pBASE, const char *pEXT)
{
    try
    {
        using v_sink_type = spdlog::sinks::daily_file_sink<std::mutex, custom_daily_file_name>;
//        v_daily_log_file = spdlog::create<v_sink_type>(pNAME, pBASE, SPDLOG_FILENAME_T((pEXT) ? pEXT : ""), 0, 1);
        v_daily_log_file = spdlog::create<v_sink_type>(pNAME, pBASE, 0, 1);
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log failed: " << ex.what() << std::endl;
    }
}

void hx_log::rotating_logger (const char *pNAME, const char *pBASE, const char *pEXT, int32_t pMAX_FILES)
{
    try
    {
        v_daily_log_file = spdlog::rotating_logger_mt (pNAME, pBASE, 1048576 * 500, pMAX_FILES);
//        v_daily_log_file = spdlog::create<spdlog::sinks::rotating_file_sink_mt> (pNAME, pBASE), 1048576 * 500, pMAX_FILES);
//        v_daily_log_file = spdlog::create<spdlog::sinks::rotating_file_sink_mt> (pNAME, pBASE, SPDLOG_FILENAME_T((pEXT) ? pEXT : ""), 1048576 * 500, pMAX_FILES);
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log failed: " << ex.what() << std::endl;
    }
}

void hx_log::set_level (spdlog::level::level_enum pLEVEL)
{
    try
    {
        //Set global log level to info
        spdlog::set_level (pLEVEL);

        v_console->set_level (pLEVEL);
        if (v_daily_log_file)
        {
            v_daily_log_file->set_level (pLEVEL);
        }
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log failed: " << ex.what() << std::endl;
    }
}

spdlog::level::level_enum hx_log::get_level (const char *pLogLevel)
{
    // "trace", "debug", "info",  "warning", "error", "critical", "off"
    spdlog::level::level_enum v_log_level = spdlog::level::off;

    if (!apr_strnatcmp ("trace", pLogLevel))
    {
        v_log_level = spdlog::level::trace;
    }
    else if (!apr_strnatcmp ("debug", pLogLevel))
    {
        v_log_level = spdlog::level::debug;
    }
    else if (!apr_strnatcmp ("info", pLogLevel))
    {
        v_log_level = spdlog::level::info;
    }
    else if (!apr_strnatcmp ("warning", pLogLevel))
    {
        v_log_level = spdlog::level::warn;
    }
    else if (!apr_strnatcmp ("error", pLogLevel))
    {
        v_log_level = spdlog::level::err;
    }
    else if (!apr_strnatcmp ("critical", pLogLevel))
    {
        v_log_level = spdlog::level::critical;
    }
    else if (!apr_strnatcmp ("off", pLogLevel))
    {
        v_log_level = spdlog::level::off;
    }

    return v_log_level;
}

hx_log::~hx_log ()
{
    try
    {
        v_console->flush ();
        if (v_daily_log_file)
        {
            v_daily_log_file->flush ();
        }

        spdlog::drop_all();
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log failed: " << ex.what() << std::endl;
    }
}
