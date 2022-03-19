#pragma once
#include "gtest/gtest.h"

#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCore.lib")
#else //_DEBUG
#pragma comment(lib, "Release\\ServerCore.lib")
#endif //_DEBUG

#include "CommonInclude.h"
