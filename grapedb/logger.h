#pragma once

#include <spdlog/spdlog.h>

namespace Grape
{

class Log
{
public:
    static void Init();

    static std::shared_ptr<spdlog::logger> &GetLogger();
};

} // namespace Grape