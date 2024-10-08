#pragma once

#include "Core.h"
#include <spdlog/spdlog.h>

#include <memory>

namespace Purple {

	class PURPLE_API Log
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
#define PRPL_CORE_FATAL(...) ::Purple::Log::GetCoreLogger()->fatal(__VA_ARGS__)
#define PRPL_CORE_ERROR(...) ::Purple::Log::GetCoreLogger()->error(__VA_ARGS__)
#define PRPL_CORE_WARN(...)  ::Purple::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define PRPL_CORE_INFO(...)  ::Purple::Log::GetCoreLogger()->info(__VA_ARGS__)
#define PRPL_CORE_TRACE(...) ::Purple::Log::GetCoreLogger()->trace(__VA_ARGS__)

// Client log
#define PRPL_FATAL(...) ::Purple::Log::GetClientLogger()->fatal(__VA_ARGS__)
#define PRPL_ERROR(...) ::Purple::Log::GetClientLogger()->error(__VA_ARGS__)
#define PRPL_WARN(...)  ::Purple::Log::GetClientLogger()->warn(__VA_ARGS__)
#define PRPL_INFO(...)  ::Purple::Log::GetClientLogger()->info(__VA_ARGS__)
#define PRPL_TRACE(...) ::Purple::Log::GetClientLogger()->trace(__VA_ARGS__)