#pragma once

#include <memory>
#include "Log.h"

#ifdef LD_ENABLE_ASSERTS

	#define LD_ASSERT(x, ...) { if(!(x)) { LD_ERROR("Assertion failed: ", __VA_ARGS__); __debugbreak(); } }
	#define LD_CORE_ASSERT(x, ...) { if(!(x)) { LD_CORE_ERROR("Assertion failed: " __VA_ARGS__); __debugbreak(); } }

#else

	#define LD_ASSERT(x, ...)
	#define LD_CORE_ASSERT(x, ...)

#endif

#define BIT(x) (1 << x)

#define LUDO_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#ifdef LD_PLATFORM_WINDOWS

#define LD_SIMD_CALLING_CONVENTION __fastcall

#else

#error Unsoported platform

#endif

namespace Ludo {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template <typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}