#include "logger.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Grape
{

std::shared_ptr<spdlog::logger> s_Logger;

void Log::Init()
{
    std::vector<spdlog::sink_ptr> sinks;

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("%^[%H:%M:%S] [%n]: %v%$");
    sinks.push_back(console_sink);

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("grapedb.log", true);
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    sinks.push_back(file_sink);

    s_Logger = std::make_shared<spdlog::logger>("GRAPE", sinks.begin(), sinks.end());

    s_Logger->set_level(spdlog::level::trace);
    s_Logger->flush_on(spdlog::level::trace);

    spdlog::register_logger(s_Logger);
    spdlog::set_default_logger(s_Logger);
}

std::shared_ptr<spdlog::logger> &Log::GetLogger()
{
    return s_Logger;
}

} // namespace Grape