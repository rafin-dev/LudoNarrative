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
// But I also dont want to regret not making it in the future
#ifdef LD_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

