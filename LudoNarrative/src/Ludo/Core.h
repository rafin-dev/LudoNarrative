#pragma once

#include "Log.h"

#ifdef LD_ENABLE_ASSERTS

	#define LD_ASSERT(x, ...) { if(!(x)) { LD_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define LD_CORE_ASSERT(x, ...) { if(!(x)) { LD_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }

#else

	#define LD_ASSERT(x, ...)
	#define LD_CORE_ASSERT(x, ...)

#endif

#define BIT(x) (1 << x)