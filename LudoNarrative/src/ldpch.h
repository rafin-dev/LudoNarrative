#pragma once

#include <iostream>
#include <memory>
#include <functional>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <filesystem>
#include <numeric>

#include <string>
#include <sstream>
#include <fstream>
#include <string_view>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <deque>
#include <stack>
#include <queue>

#include "Ludo/Core/Core.h"
#include "Ludo/Core/Log.h"
#include "Ludo/Debug/Instrumentor.h"

// LudoNarrative is planned to be windows only, I feel sily doing things for multiplatform projects
// But I also dont want to regret not making this way in the future
#ifdef LD_PLATFORM_WINDOWS
	#define NOMINMAX
	#include <Windows.h>
	#include <wincodec.h>
	#include <windowsx.h>

	// DirectX11
	#include <d3d11.h>
	#include <d3d10.h>
	#include <dxgi1_6.h>
	#include <d3dcompiler.h>

	#pragma comment(lib, "d3d11.lib")
	#pragma comment (lib, "dxgi.lib")
	#pragma comment(lib, "dxguid.lib")
	#pragma comment(lib, "d3dcompiler")

	// DirectX12
	#include <d3d12.h>
	#include <dxcapi.h>

	#ifdef LUDO_DEBUG

		#include <d3d12sdklayers.h>
		#include <dxgidebug.h>

	#endif

	#pragma comment(lib, "d3d12.lib")
	#pragma comment(lib, "dxgi.lib")
	#pragma comment(lib, "dxguid.lib")
	#pragma comment(lib, "d3dcompiler.lib")
	#pragma comment(lib, "dxcompiler.lib")

	#include <DirectXMath.h>
	#include <DirectXPackedVector.h>
	#include <DirectXColors.h>
	#include <DirectXCollision.h>
#endif

