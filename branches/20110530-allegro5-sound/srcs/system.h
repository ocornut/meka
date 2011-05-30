//-----------------------------------------------------------------------------
// MEKA - system.h
// Base definitions, system and libraries includes, etc..
//-----------------------------------------------------------------------------

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
#ifndef ARCH_UNIX
  #ifndef ARCH_WIN32
    #include <dir.h>
    #include <conio.h>
    #include <dos.h>
    #include <dir.h>
    #include <dpmi.h>
    #include <io.h>
    #include <sys/segments.h>
    #include <sys/nearptr.h>
  #endif
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

//#ifndef ALLEGRO_STATICLINK
//    #define ALLEGRO_STATICLINK
//#endif
#define alleg_flic_unused
#define alleg_sound_unused
#define alleg_gui_unused
#define alleg_math_unused
#define ALLEGRO_NO_COMPATIBILITY
#ifdef ARCH_WIN32
#define ALLEGRO_STATICLINK			// Unix users probably don't want static linking?
#endif

// FIXME-OPT: Because of bug in 4.2.* branch
// What we need is to switch to 4.3.* but I cannot find precompiled binaries for that
// See http://www.allegro.cc/forums/print-thread/598240
#define ALLEGRO_NO_ASM		

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include "allegro4to5.h"

#undef MSG_USER             // To avoid mistyping MSG_USER instead of MSGT_USER. We don't use Allegro GUI anyway.

#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))

//-----------------------------------------------------------------------------
// Basic Types
//-----------------------------------------------------------------------------

#ifndef _BASE_TYPES
#define _BASE_TYPES

#define BYTE_TYPE_DEFINED // for z80marat/z80.h
#define WORD_TYPE_DEFINED // for z80marat/z80.h

// Obsolete. Avoid use
typedef unsigned char       UC;
typedef unsigned char       byte;
typedef unsigned short      word;
typedef unsigned long       dword;

// Data types - use when size matter
typedef unsigned char       u8;
typedef   signed char       s8;
typedef unsigned short      u16;
typedef   signed short      s16;
typedef unsigned int        u32;
typedef   signed int        s32;

#ifndef ARCH_WIN32
    typedef long long       s64;
#else
    typedef LONGLONG        s64;
#endif

/*
// FIXME: It is the right way to define 'bool' ? 
// Remove when switching to C++
typedef int                 bool;
*/

#endif /* #ifndef _BASE_TYPES */

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

#ifndef ARCH_WIN32
    #define FORMAT_PRINTF(START)  __attribute ((format (printf, START, (START)+1)))
#else
    #define FORMAT_PRINTF(START)
#endif

//-----------------------------------------------------------------------------
// Miscellaneous
//-----------------------------------------------------------------------------

#ifdef ARCH_WIN32
  #define snprintf      _snprintf
#else
#endif
#ifdef ARCH_UNIX
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
