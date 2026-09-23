#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Logger::s_Logger;

void Logger::Init()
{
    s_Logger = spdlog::stdout_color_mt("ValorantBot");
    s_Logger->set_level(spdlog::level::trace);
}
