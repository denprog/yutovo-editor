#include "logger.h"
#include <filesystem>

namespace yutovo
{

//Logger

Logger::Logger()
{
    std::filesystem::create_directory("log");
    try
    {
        log = spdlog::daily_logger_mt("editor", "log/editor.log", 0, 0);
    }
    catch (spdlog::spdlog_ex& ex)
    {
    }

    spdlog::set_pattern("[%H:%M:%S.%e][%t][%n][%l] %v");
    spdlog::set_level(spdlog::level::debug);
}

Logger* Logger::GetInstance()
{
    static Logger log;
    return &log;
}

void Logger::Info(const char* message)
{
    if (!log)
        return;
    log->info(message);
    log->flush();
}

void Logger::Debug(const char* message)
{
    if (!log)
        return;
    log->debug(message);
    log->flush();
}

void Logger::Warning(const char* message)
{
    if (!log)
        return;
    log->warn(message);
    log->flush();
}

void Logger::Error(const char* message)
{
    if (!log)
        return;
    log->error(message);
    log->flush();
}

}
