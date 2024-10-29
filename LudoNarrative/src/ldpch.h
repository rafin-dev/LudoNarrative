#pragma once

#include <iostream>
#include <memory>
#include <functional>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <locale>
#include <codecvt>

#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <deque>
#include <stack>
#include <queue>

// LudoNarrative is planned to be windows only, I feel sily doing things for multiplatform projects
// But I also dont want to regret not making this way in the future
#ifdef LD_PLATFORM_WINDOWS
	#include <Windows.h>
	#include <windowsx.h>
	
	/*#include <d3d9.h>
	#include <d3d11.h>
	#include <d3d11_4.h>

	#pragma comment (lib, "d3d9.lib")
	#pragma comment (lib, "d3d11.lib")*/

	// DirectX12
	#include <d3d12.h>
	#include <dxgi1_6.h>

	#ifdef LUDO_DEBUG

		#include <d3d12sdklayers.h>
		#include <dxgidebug.h>

	#endif

	#include <wrl.h>

	#pragma comment(lib, "d3d12.lib")
	#pragma comment (lib, "dxgi.lib")
	#pragma comment(lib, "dxguid.lib")

	#include <DirectXMath.h>
	#include <DirectXPackedVector.h>
	#include <DirectXColors.h>
	#include <DirectXCollision.h>
#endif

