#pragma once

// standard
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <thread>
#include <algorithm>
#include <type_traits>
#include <numbers>
#include <chrono>
#include <numeric>
#include <functional>
#include <fstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
#include <atomic>
#include <string>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <stack>
#include <unordered_map>

// os
#include <Windows.h>

#include "Types.h"
#include "Definition.h"
#include "CommonGlobal.h"
#include "CommonThreadLocal.h"
#include "Lock.h"
#include "LockGuard.h"
#include "Container.h"

using namespace std::literals;
using namespace std::string_literals;
using namespace std::string_view_literals;
/*
 * precompiled header에도 포함하며 Client,Server 프로젝트가 동시에 참조합니다.
 */


