//-----------------------------------------------------------------------------
// MEKA - system.h
// Base definitions, system and libraries includes, etc..
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Architecture
//-----------------------------------------------------------------------------

#ifdef _WIN32
#define ARCH_WIN32		(1)
#endif

//-----------------------------------------------------------------------------
// STL
//-----------------------------------------------------------------------------

#define _HAS_EXCEPTIONS 0
#define _STATIC_CPPLIB
#define _DISABLE_DEPRECATE_STATIC_CPPLIB
#include <vector>
#include <map>
#include <algorithm>

//-----------------------------------------------------------------------------
// System Includes
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#ifndef ARCH_MACOSX
  #include <malloc.h>
#endif // ARCH_MACOSX
#include <math.h>
#include <string.h>
#include <float.h>
#include <sys/stat.h>
#include <ctype.h>
#include <assert.h>
#ifndef ARCH_WIN32
  #include <dirent.h>
#endif
#ifdef ARCH_WIN32
	#define stricmp _stricmp
    #define ALLEGRO_BITMAP WINDOWS_BITMAP
    #include <windows.h>
    #include <crtdbg.h>
    #undef ALLEGRO_BITMAP
    #undef RGB
#endif

//-----------------------------------------------------------------------------
// Allegro Library
//-----------------------------------------------------------------------------

#ifdef ARCH_WIN32
#define ALLEGRO_STATICLINK			// Unix users probably don't want static linking?
#endif	

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_ttf.h>
#ifdef ARCH_WIN32
#include <allegro5/allegro_windows.h>
#endif
#include "allegro4to5.h"

#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))

//-----------------------------------------------------------------------------
// Basic Types
//-----------------------------------------------------------------------------

#define BYTE_TYPE_DEFINED // for z80marat/z80.h
#define WORD_TYPE_DEFINED // for z80marat/z80.h

// Obsolete, avoid use
typedef unsigned char       byte;
typedef unsigned short      word;
typedef unsigned long       dword;

typedef unsigned char       u8;
typedef   signed char       s8;
typedef unsigned short      u16;
typedef   signed short      s16;
typedef unsigned int        u32;
typedef   signed int        s32;
typedef unsigned long long  u64;
typedef   signed long long  s64;

//-----------------------------------------------------------------------------
// Inlining
//-----------------------------------------------------------------------------

#ifndef INLINE
    #ifdef ARCH_WIN32
        #define INLINE __inline
    #else
        #define INLINE inline
    #endif
#endif

//-----------------------------------------------------------------------------
// Format function attributes
//-----------------------------------------------------------------------------
// FIXME: this is for GCC only. Is there an equivalent for Visual C++ ?
//-----------------------------------------------------------------------------

#if !defined(ARCH_WIN32) && !defined(ARCH_MACOSX)
    #define FORMAT_PRINTF(START)  __attribute ((format (printf, START, (START)+1)))
#else
    #define FORMAT_PRINTF(START)
#endif

//-----------------------------------------------------------------------------
// Miscellaneous
//-----------------------------------------------------------------------------

#undef FALSE
#define FALSE (0)

#undef TRUE
#define TRUE (1)

#ifdef ARCH_WIN32
  #define snprintf      _snprintf
#else
#endif
#if defined(ARCH_UNIX) || defined(ARCH_MACOSX)
  #define stricmp strcasecmp
  #define strnicmp strncasecmp
#endif

// countof(): provide number of elements in an array
// The simple version is:
//   #define countof(array) (sizeof(array) / sizeof(array[0]))
// This more complicated version ensure that an array (not a pointer) is actually provided as the parameter
// From http://blogs.msdn.com/the1/archive/2004/05/07/128242.aspx
//template <typename T, size_t N>
//char (&_ArraySizeHelper(T (&array)[N])) [N];
//#define countof(array) (sizeof(_ArraySizeHelper(array)))
#define countof(array)	(sizeof(array) / sizeof(array[0]))

#define MATH_PI		(3.14159265f)

//-----------------------------------------------------------------------------
// Warning Disable
//-----------------------------------------------------------------------------
// Now compiling at warning level 4, so a few can be omitted.
//-----------------------------------------------------------------------------

#ifdef ARCH_WIN32
#pragma warning (disable: 4100) // 'unreferenced formal parameter'
#pragma warning (disable: 4127) // 'conditional expression is constant'
#pragma warning (disable: 4996) // ''_snprintf': This function or variable may be unsafe'
//#define _CRT_SECURE_NO_WARNINGS
#endif

//-----------------------------------------------------------------------------
