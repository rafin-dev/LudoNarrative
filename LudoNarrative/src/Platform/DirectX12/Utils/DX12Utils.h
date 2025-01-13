#pragma once

#include "Ludo/Core/Core.h"

// Used inside initialization functions
#define VALIDATE_DX12_HRESULT(x, ...) \
		if (FAILED(x)) \
		{ \
			LD_CORE_ASSERT(false, __VA_ARGS__); \
			ShutDown(); \
			return false; \
		}

// used anywhere during runtime
#pragma warning( disable : 4390 )
#define CHECK_DX12_HRESULT(x, ...)	LD_CORE_ASSERT(SUCCEEDED(x), __VA_ARGS__) \
									if (FAILED(x))

#define CHECK_AND_RELEASE_COMPTR(x) if (x != nullptr) { x->Release(); x = nullptr; }