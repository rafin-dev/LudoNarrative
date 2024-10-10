#pragma once

#ifdef LD_PLATFORM_WINDOWS
	#ifdef LD_BUILD_DLL
		#define LUDO_API __declspec(dllexport)
	#else
		#define LUDO_API __declspec(dllimport)
	#endif
#else
	#error Ludo Engine is windows only!
#endif

#ifdef LD_ENABLE_ASSERTS

	#define LD_ASSERT(x, ...) { if(!(x)) { LD_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define LD_CORE_ASSSERT(x, ...) { if(!(x)) { LD_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }

#else

	#define LD_ASSERT(x, ...)
	#define LD_CORE_ASSERT(x, ...)

#endif

#define BIT(x) (1 << x)