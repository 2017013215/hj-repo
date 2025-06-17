
//--------------------------------------------------------------------------------
// 作者  皮学贤 
// 日期：2010-05-26 
// 功能：基础类库之基本头文件
//================================================================================

#pragma once

#ifdef ARES_BASE_EXPORTS
#define ARS_BASE_API __declspec(dllexport)
#else
#define ARS_BASE_API /*__declspec(dllimport)*/
#endif

// 常用头文件
#include <math.h>
#include <windows.h>

// STL algorithms & functions
#include <algorithm>
#include <functional>
#include <limits>
#include <map>
#include <set>
#include <list>
#include <queue>
#include <string>
#include <ostream>

namespace ArsBase
{
	//--------------------------------------------------------------------------------
	// 作者  皮学贤 
	// 日期：2010-06-04 
	// 功能：浮点数-以双浮点为主
	//================================================================================
	typedef double Real;
}

#ifdef ARES_BASE_EXPORTS
#ifdef _DEBUG
#include <stdlib.h>
#include <crtdbg.h>
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#endif
