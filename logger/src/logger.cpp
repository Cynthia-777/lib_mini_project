#include "../include/logger.h"

std::shared_ptr<spdlog::logger> Logger::daily_logger;

void Logger::init_logger() {
    daily_logger = spdlog::daily_logger_format_mt("http_logger", "../logs/my_log", 2, 30);
}