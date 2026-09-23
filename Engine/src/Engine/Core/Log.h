#pragma once

#include <spdlog/spdlog.h>

#include <memory>

class Logger
{
public:
    static void Init();

    static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

private:
    static std::shared_ptr<spdlog::logger> s_Logger;
};

#define TRACE(...)		::Logger::GetLogger()->trace(__VA_ARGS__)
#define INFO(...)		::Logger::GetLogger()->info(__VA_ARGS__)
#define WARN(...)		::Logger::GetLogger()->warn(__VA_ARGS__)
#define ERR(...)		::Logger::GetLogger()->error(__VA_ARGS__)
#define CRITICAL(...)	::Logger::GetLogger()->critical(__VA_ARGS__)