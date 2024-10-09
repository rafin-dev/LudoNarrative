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

#define BIT(x) (1 << x)