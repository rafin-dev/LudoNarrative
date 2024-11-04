#pragma once

#include "Ludo/Log.h"

// Made to be used by any D3D12 implementation class, x is a HRESULT, either a variable or the function call itself
// Make sure to only use inside methods that return a boolean with the succes value and that the class implements ShutDown()
#define VALIDATE_DXCALL_SUCCESS(x, ...); \
	if (FAILED(x))\
	{\
		LD_CORE_ERROR(__VA_ARGS__);\
		ShutDown();\
		return false;\
	}

// Self explanatory
#define CHECK_AND_RELEASE_COMPTR(x) if (x != nullptr) { x->Release(); x = nullptr; }

// Huge thanks to https://www.youtube.com/@L%C3%B6twigFusel for the D3D12Ez series