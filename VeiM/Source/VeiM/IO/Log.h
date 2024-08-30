#pragma once

#include "VeiM/Core/CoreDefines.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace VeiM
{
	class Log
	{
	public:
		static void Init();

		static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;

	};

// Core
#define VM_CORE_TRACE(...)		::VeiM::Log::GetCoreLogger()->trace(__VA_ARGS__) 
#define VM_CORE_INFO(...)		::VeiM::Log::GetCoreLogger()->info(__VA_ARGS__)
#define VM_CORE_WARN(...)		::VeiM::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define VM_CORE_ERROR(...)		::VeiM::Log::GetCoreLogger()->error(__VA_ARGS__)
#define VM_CORE_CRITICAL(...)	::VeiM::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client
#define VM_TRACE(...)			::VeiM::Log::GetClientLogger()->trace(__VA_ARGS__)
#define VM_INFO(...)			::VeiM::Log::GetClientLogger()->info(__VA_ARGS__)
#define VM_WARN(...)			::VeiM::Log::GetClientLogger()->warn(__VA_ARGS__)
#define VM_ERROR(...)			::VeiM::Log::GetClientLogger()->error(__VA_ARGS__)
#define VM_CRITICAL(...)		::VeiM::Log::GetClientLogger()->critical(__VA_ARGS__)
}