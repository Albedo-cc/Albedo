#include "log.h"

#include <spdlog/common.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Albedo
{

	Log::Logger::Logger()
	{
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        //auto log_file = std::filesystem::current_path().append("trace.log").string();
        //auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file, true);
        //file_sink->set_level(spdlog::level::trace);

        m_spdlog = new spdlog::logger("Albedo Log", { console_sink/*file_sink*/ });
#ifndef NDEBUG // Debug
        m_spdlog->set_level(spdlog::level::debug);
#else
        m_spdlog->set_level(spdlog::level::warn);
#endif
        //m_spdlog->set_pattern("[%^%l%$] [%Y-%m-%d %H:%M:%S] %v");
        m_spdlog->set_pattern("%^[%l - %H:%M:%S - Thread:%t]%$\n>> %v");
	    m_spdlog->debug("Albedo Log System is begin initialized.");
    }

    Log::Logger::~Logger()
    {
        m_spdlog->debug("Albedo Log System is being terminated...");
        m_spdlog->flush();
        spdlog::drop_all();
        delete m_spdlog;
    }

} // namespace Albedo