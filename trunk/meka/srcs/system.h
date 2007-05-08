//-----------------------------------------------------------------------------
// MEKA - system.h
// Base definitions, system and libraries includes, etc..
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// System Includes
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#ifndef MACOSX
  #include <malloc.h>
#endif // MACOSX
#include <math.h>
#include <string.h>
#include <float.h>
#include <sys/stat.h>
#include <ctype.h>
#include <assert.h>
#ifndef WIN32
  #include <dirent.h>
#endif
#ifndef UNIX
  #ifndef WIN32
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
#ifdef WIN32
    #define BITMAP WINDOWS_BITMAP
    #include <windows.h>
    #include <crtdbg.h>
    #undef BITMAP
    #undef RGB
#endif

//-----------------------------------------------------------------------------
// Allegro Library
//-----------------------------------------------------------------------------

#ifndef ALLEGRO_STATICLINK
    #define ALLEGRO_STATICLINK
#endif
#define alleg_flic_unused
#define alleg_sound_unused
#define alleg_gui_unused
#define alleg_math_unused
#define ALLEGRO_NO_COMPATIBILITY
#include <allegro.h>
#undef MSG_USER             // To avoid mistyping MSG_USER instead of MSGT_USER. We don't use Allegro GUI anyway.

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

#ifndef WIN32
    typedef long long       s64;
#else
    typedef LONGLONG        s64;
#endif

// FIXME: It is the right way to define 'bool' ? 
// Remove when switching to C++
typedef int                 bool;

#endif /* #ifndef _BASE_TYPES */

//-----------------------------------------------------------------------------
// Inlining
//-----------------------------------------------------------------------------

#ifndef INLINE
    #ifdef WIN32
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

#ifndef WIN32
    #define FORMAT_PRINTF(START)  __attribute ((format (printf, START, (START)+1)))
#else
    #define FORMAT_PRINTF(START)
#endif

//-----------------------------------------------------------------------------
// Miscellaneous
//-----------------------------------------------------------------------------

#ifdef WIN32
  #define meka_mkdir(a) mkdir(a);
  #define snprintf      _snprintf
#else
  #define meka_mkdir(a) mkdir(a, 0700);
#endif
#ifdef UNIX
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

#ifdef WIN32
#pragma warning (disable: 4100) // 'unreferenced formal parameter'
#pragma warning (disable: 4127) // 'conditional expression is constant'
#endif

//-----------------------------------------------------------------------------
