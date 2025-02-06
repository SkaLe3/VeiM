#include "Logging/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>


namespace VeiM
{
	SharedPtr<spdlog::logger> Log::s_CoreLogger;
	SharedPtr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(MakeShared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(MakeShared<spdlog::sinks::basic_file_sink_mt>("VeiM_run.log", true));

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		spdlog::level::level_enum loglevel;
#ifdef VM_LOG_LEVEL_DEBUG
		loglevel = spdlog::level::trace;
#else
		loglevel = spdlog::level::info;
#endif;

		s_CoreLogger = MakeShared<spdlog::logger>("VEIM", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_CoreLogger);
		s_CoreLogger->set_level(spdlog::level::trace);
		s_CoreLogger->flush_on(spdlog::level::trace);

		s_ClientLogger = MakeShared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_ClientLogger);
		s_ClientLogger->set_level(spdlog::level::trace);
		s_ClientLogger->flush_on(spdlog::level::trace);
	}
}