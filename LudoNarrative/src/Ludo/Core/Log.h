#pragma once

#include "Core.h"
#include <spdlog/spdlog.h>

namespace Ludo {

	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:

		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// Engine log
#define LD_CORE_ERROR(...) ::Ludo::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LD_CORE_WARN(...)  ::Ludo::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LD_CORE_INFO(...)  ::Ludo::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LD_CORE_TRACE(...) ::Ludo::Log::GetCoreLogger()->trace(__VA_ARGS__)

// Client log
#define LD_ERROR(...) ::Ludo::Log::GetClientLogger()->error(__VA_ARGS__)
#define LD_WARN(...)  ::Ludo::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LD_INFO(...)  ::Ludo::Log::GetClientLogger()->info(__VA_ARGS__)
#define LD_TRACE(...) ::Ludo::Log::GetClientLogger()->trace(__VA_ARGS__)