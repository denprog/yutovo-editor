#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "spdlog/spdlog.h"
#include "spdlog/sinks/daily_file_sink.h"

namespace yutovo
{

class Logger
{
private:
    Logger();

public:
    Logger(Logger const&) = delete;
    void operator=(Logger const&) = delete;

    static Logger* GetInstance();

    void Info(const char* message);
    void Debug(const char* message);
    void Warning(const char* message);
    void Error(const char* message);

    template<typename... Args>
    void Info(const char* format, Args... args)
    {
        log->info(format, args...);
        log->flush();
    }

    template<typename... Args>
    void Debug(const char* format, Args... args)
    {
        log->debug(format, args...);
        log->flush();
    }

    template<typename... Args>
    void Warning(const char* format, Args... args)
    {
        log->warn(format, args...);
        log->flush();
    }

    template<typename... Args>
    void Error(const char* format, Args... args)
    {
        log->error(format, args...);
        log->flush();
    }

private:
    std::shared_ptr<spdlog::logger> log;
};

}

#endif
