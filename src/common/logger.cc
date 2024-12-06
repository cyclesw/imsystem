
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "logger.h"

std::shared_ptr<spdlog::logger> ns_log::Logger::_logger = nullptr;
std::unordered_map<std::string, spdlog::sink_ptr> ns_log::Logger::_sinks;

void ns_log::Logger::InitLogger(bool mode, LogLevel level, const std::string &pattern, const std::string &file)
{
    if (!mode)
    {
        _logger = spdlog::stdout_color_mt("console");
    }
    else
    {
        _logger = spdlog::basic_logger_mt("console", file);
    }

    _logger->set_level(level);
    _logger->set_pattern(pattern);
    spdlog::set_default_logger(_logger);
}

void ns_log::Logger::RegisterSink(const std::string& logger_name, const LogSetting &setting)
{
    spdlog::sink_ptr sink;
    switch (setting.Type)
    {
        case LogType::Stdout:
        {
            sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        }
        case LogType::Stderr:
        {
            sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
        }
        case LogType::File:
        {
            sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(setting.FileName,
                                                                              setting.Truncate,
                                                                              setting.Handle);
        }
        case LogType::Daily:
        {
            sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
                setting.FileName,
                setting.Hours,
                setting.Minutes,
                setting.Truncate,
                setting.FileSize,
                setting.Handle);
        }
        case LogType::Rotating:
        {
            sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                setting.FileName,
                setting.MaxSize,
                setting.FileSize,
                false,
                setting.Handle);
        }
        default: throw std::invalid_argument("Invalid log type");
        _logger->sinks().emplace_back(sink);
        _sinks.emplace(logger_name, sink);
    }
}

void ns_log::Logger::RemoveSink(const std::string &name)
{
    const auto ret = std::ranges::find_if(_logger->sinks(), [&]
        (const std::shared_ptr<spdlog::sinks::sink> &sink)
    {
        return sink == _sinks[name];
    });

    if (ret != _logger->sinks().end())
        _logger->sinks().erase(ret);
}
