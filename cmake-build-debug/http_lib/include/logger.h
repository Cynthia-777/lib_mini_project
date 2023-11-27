#ifndef HTTP_LIB_LOGGER_H
#define HTTP_LIB_LOGGER_H

#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>

class Logger{
public:
    static std::shared_ptr<spdlog::logger> daily_logger;
    static void init_logger();
};

#endif //HTTP_LIB_LOGGER_H
