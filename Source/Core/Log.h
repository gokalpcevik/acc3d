#pragma once
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace acc3d
{
    namespace Core
    {
        class Log
        {
        private:
            static std::shared_ptr<spdlog::logger> s_Logger;

        public:
            Log() = default;
            static void Init();
            static auto GetLogger() -> std::shared_ptr<spdlog::logger> & { return s_Logger; }
        };
    }
}

#define acc3d_trace(...) ::acc3d::Core::Log::GetLogger()->trace(__VA_ARGS__)
#define acc3d_debug(...) ::acc3d::Core::Log::GetLogger()->debug(__VA_ARGS__)
#define acc3d_info(...) ::acc3d::Core::Log::GetLogger()->info(__VA_ARGS__)
#define acc3d_warn(...) ::acc3d::Core::Log::GetLogger()->warn(__VA_ARGS__)
#define acc3d_error(...) ::acc3d::Core::Log::GetLogger()->error(__VA_ARGS__)
#define acc3d_critical(...) ::acc3d::Core::Log::GetLogger()->critical(__VA_ARGS__)