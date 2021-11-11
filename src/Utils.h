#pragma once

#include <spdlog/spdlog.h>

#include <spdlog/sinks/stdout_color_sinks.h>

namespace SiFi
{

using logger = std::shared_ptr<spdlog::logger>;

namespace log = spdlog;

inline logger createLogger(const std::string& name)
{
    auto alreadyExists = spdlog::get(name);
    if (alreadyExists != nullptr) { return alreadyExists; }

    auto _logger = spdlog::stdout_color_st(name);
    spdlog::drop(name); // it will be released after all references are lost
    return _logger;
}

} // namespace SiFi
