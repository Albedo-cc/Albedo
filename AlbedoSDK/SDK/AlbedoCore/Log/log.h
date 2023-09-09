#pragma once

#include <spdlog/spdlog.h>

namespace Albedo
{

	class Log
	{
    // Interface
    public:
        template <typename... Args>
	    using format_string = spdlog::format_string_t<Args...>;

        template <typename... Args>
        static inline void Debug(format_string<Args...> fmt, Args&&... args)
        {
            m_logger->debug(fmt, std::forward<Args>(args)...);
        }

		template <typename... Args>
        static inline void Info(format_string<Args...> fmt, Args&&... args)
        {
            m_logger->info(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static inline void Warn(format_string<Args...> fmt, Args&&... args)
        {
            m_logger->warn(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static inline void Error(format_string<Args...> fmt, Args&&... args)
        {
            auto msg = fmt::format(fmt, std::forward<Args>(args)...);
            m_logger->error(msg);
            throw std::runtime_error(msg);
        }

        template <typename... Args>
        static inline void Fatal(format_string<Args...> fmt, Args&&... args)
        {
            m_logger->critical(fmt, std::forward<Args>(args)...);
            std::terminate(); // Note!
        }

    // Implement
    private:
        class Logger
        {
        public:
            Logger();
            ~Logger();
            spdlog::logger* operator ->() { return m_spdlog; }
        private:
            spdlog::logger* m_spdlog = nullptr;
        };
        static inline Logger m_logger; // Init in static stage.
	};

} // namespace Albedo