#pragma once

#include "Ludo/Log.h"

#define VALIDATE_DX_HRESULT(x, ...) \
						if (FAILED((x))) \
						{ \
							LD_CORE_ERROR(__VA_ARGS__);\
							ShutDown();\
							return false;\
						}

#define CHECK_AND_RELEASE_COMPTR(x) if (x != nullptr) { x->Release(); x = nullptr; }