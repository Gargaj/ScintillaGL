/*******************************************************************************
*   2005-2014, plastic demoscene group
*	authors: misz, bonzaj
*******************************************************************************/

#pragma once

#include "targetver.h"

#if defined(_MSC_VER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#else
#include <stdlib.h>
#endif

#include <stdio.h>
#include <direct.h>
#include <assert.h>
#include <sstream>
#include <vector>
#include <list>
#include <process.h>

#include <windows.h>

// #include "../../external/glew/include/GL/glew.h"
// #include "../../external/glew/include/GL/wglew.h"
// #include "../../external/freeglut/include/GL/freeglut.h"
// #include "../../external/libpng/png.h"
// #include "../../external/libconfig/lib/libconfig.h"
 #include "../bass/bass.h"

#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#if defined(_MSC_VER) && defined(_DEBUG)
	/**
	 *	Be aware to add:
	 *		#if defined(_MSC_VER) && defined(_DEBUG)
	 *		#define new _DEBUG_NEW
	 *		#endif
	 *	at the beginning of a file that uses 'new' operator. Only then overloaded versions for tracking allocation
	 *	will work!!!
	 */
	#define _DEBUG_NEW   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
	#define _DEBUG_NEW
#endif //
