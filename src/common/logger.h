//
// Created by lang liu on 2024/12/5.
//

#ifndef LOGGER_H
#define LOGGER_H


#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include "source_message.h"

namespace ns_log
{

    using LogLevel = spdlog::level::level_enum;
    using FileEventHandle = spdlog::file_event_handlers;

    enum LogType
    {
        Stdout = 0,
        Stderr,
        File,
        Daily,
        Rotating
    };
    

    struct LogSetting
    {
        LogType Type;
        FileEventHandle Handle;
        std::string FileName;
        uint8_t Hours;
        uint8_t Minutes;
        unsigned int FileSize;
        unsigned long MaxSize;
        bool Truncate;
    };

    class Logger final
    {
    private:
        static std::shared_ptr<spdlog::logger> _logger;
        static std::unordered_map<std::string, spdlog::sink_ptr> _sinks;
    public:
        static void InitLogger(bool mode, LogLevel level, const std::string& pattern = "", const std::string &file = "");

        static void RegisterSink(const std::string& logger_name, const LogSetting &setting);

        static void RemoveSink(const std::string& name);

        template <typename... Args>
        static void Log(SourceLocation loc, LogLevel level, fmt::format_string<Args...> fmt, Args &&... args);

        template <typename T>
        static void Log(SourceLocation loc, LogLevel level, const T& val);

        // Trace
        template <typename... Args>
        static void Trace(SourceLocation loc, fmt::format_string<Args...> fmt, Args &&... args);

        // Debug
        template <typename... Args>
        static void Debug(SourceLocation loc, fmt::format_string<Args...> fmt, Args &&... args);

        // Info
        template <typename... Args>
        static void Info(SourceLocation loc, fmt::format_string<Args...> fmt, Args &&... args);

        // Warn
        template <typename... Args>
        static void Warn(SourceLocation loc, fmt::format_string<Args...> fmt, Args &&... args);

        // Error
        template <typename... Args>
        static void Error(SourceLocation loc, fmt::format_string<Args...> fmt, Args &&... args);

        // Critical
        template <typename... Args>
        static void Critical(SourceLocation loc, fmt::format_string<Args...> fmt, Args &&... args);
    };

    template<typename ... Args>
    void Logger::Log(SourceLocation loc, LogLevel level, fmt::format_string<Args...> fmt, Args &&...args)
    {
        spdlog::log(GetLogSourceLocation(loc), level, fmt, std::forward<Args>(args)...);
    }

    template<typename T>
    void Logger::Log(SourceLocation loc, LogLevel level, const T &val)
    {
        spdlog::log(GetLogSourceLocation(loc), level, val);
    }

    template<typename ... Args>
    void Logger::Trace(SourceLocation loc, fmt::format_string<Args...> fmt, Args &&...args)
    {
        spdlog::log(GetLogSourceLocation(loc), spdlog::level::trace, fmt, std::forward<Args>(args)...);
    }

    template<typename ... Args>
    void Logger::Debug(SourceLocation loc, fmt::format_string<Args...> fmt, Args &&...args)
    {
        spdlog::log(GetLogSourceLocation(loc), spdlog::level::debug, fmt, std::forward<Args>(args)...);
    }

    template<typename ... Args>
    void Logger::Info(SourceLocation loc, fmt::format_string<Args...> fmt, Args &&...args)
    {
        spdlog::log(GetLogSourceLocation(loc), spdlog::level::info, fmt, std::forward<Args>(args)...);
    }

    template<typename ... Args>
    void Logger::Warn(SourceLocation loc, fmt::format_string<Args...> fmt, Args &&...args)
    {
        spdlog::log(GetLogSourceLocation(loc), spdlog::level::warn, fmt, std::forward<Args>(args)...);
    }

    template<typename ... Args>
    void Logger::Error(SourceLocation loc, fmt::format_string<Args...> fmt, Args &&...args)
    {
        spdlog::log(GetLogSourceLocation(loc), spdlog::level::err, fmt, std::forward<Args>(args)...);
    }

    template<typename ... Args>
    void Logger::Critical(SourceLocation loc, fmt::format_string<Args...> fmt, Args &&...args)
    {
        spdlog::log(GetLogSourceLocation(loc), spdlog::level::critical, fmt, std::forward<Args>(args)...);
    }

#if __cplusplus >= 202002L


#define LOG_TRACE(...)       ns_log::Logger::Log({}, ns_log::LogLevel::trace, __VA_ARGS__)
#define LOG_DEBUG(...)       ns_log::Logger::Log({}, ns_log::LogLevel::debug, __VA_ARGS__)
#define LOG_INFO(...)        ns_log::Logger::Log({}, ns_log::LogLevel::info, __VA_ARGS__)
#define LOG_WARN(...)        ns_log::Logger::Log({}, ns_log::LogLevel::warn, __VA_ARGS__)
#define LOG_ERROR(...)       ns_log::Logger::Log({}, ns_log::LogLevel::err, __VA_ARGS__)
#define LOG_CRITICAL(...)    ns_log::Logger::Log({}, ns_log::LogLevel::critical, __VA_ARGS__)

#else

#define LOG_TRACE(...)      SPDLOG_LOGGER_TRACE(spdlog::default_logger(), __VA_ARGS__)
#define LOG_DEBUG(...)      SPDLOG_LOGGER_DEBUG(spdlog::default_logger(), __VA_ARGS__)
#define LOG_INFO(...)       SPDLOG_LOGGER_INFO(spdlog::default_logger(), __VA_ARGS__)
#define LOG_WARN(...)       SPDLOG_LOGGER_WARN(spdlog::default_logger(), __VA_ARGS__)
#define LOG_ERROR(...)      SPDLOG_LOGGER_ERROR(spdlog::default_logger(), __VA_ARGS__)
#define LOG_CRITICAL(...)   SPDLOG_LOGGER_CRITICAL(spdlog::default_logger(), __VA_ARGS__)

#endif

}

#endif //LOGGER_H