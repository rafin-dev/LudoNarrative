#pragma once

#ifdef PRPL_PLATFORM_WINDOWS
	#ifdef PRPL_BUILD_DLL
		#define PURPLE_API __declspec(dllexport)
	#else
		#define PURPLE_API __declspec(dllimport)
	#endif
#else
	#error Purple is windows only!
#endif