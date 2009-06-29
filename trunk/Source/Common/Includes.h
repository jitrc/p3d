#pragma once

#define P3D_WINDOWS

#pragma region ("Disabling of these specific warnings")
#pragma warning(disable:4290)             
#pragma warning(disable:4355) 
#pragma warning(disable:4482)
#pragma warning(disable:4251)
#pragma warning(disable:4661)
#pragma warning(disable:4996)
#pragma endregion

// enable recursive __forceinlines
#pragma inline_recursion(on)
#pragma inline_depth(256)

#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

//// CRT
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>

#include <stdarg.h>
#include <time.h>

// STD
#include <typeinfo>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <set>
#include <hash_map>
#include <algorithm>
#include <stack>
#include <sstream>
#include <fstream>
#include <cmath>
#include <queue>

namespace std
{
    using namespace ::stdext;
}

#include "Common/DataTypes.h"

#define ALIGNED_VARIABLE(alignment, type, name) __declspec(align(alignment)) type name;

#define override virtual
#define forceinline __forceinline

#ifdef MAX_PATH
#undef MAX_PATH
#endif
#define MAX_PATH 260

#pragma region("CRT Memory Leak detection definitions")
#ifndef ASSERT
#ifdef _DEBUG
#define ASSERT _ASSERT
#else
#define ASSERT(expr) 0
#endif
#endif

#define _CRT_SECURE_NO_WARNINGS

#ifdef   _DEBUG

#define  SET_CRT_DEBUG_FIELD(a) \
            _CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#define  CLEAR_CRT_DEBUG_FIELD(a) \
            _CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else
#define  SET_CRT_DEBUG_FIELD(a)   ((void) 0)
#define  CLEAR_CRT_DEBUG_FIELD(a) ((void) 0)
#endif

// Use {,,msvcr80d.dll}_crtBreakAlloc watch variable to set auto break on allocation index
static inline void EnableCrtCheck() { SET_CRT_DEBUG_FIELD(_CRTDBG_LEAK_CHECK_DF); }
#pragma endregion

// Used to step into the debugger
#define  DebuggerBreak()  __asm { int 3 }

// Used for dll exporting and importing
#define  DLL_EXPORT   extern "C" __declspec( dllexport ) 
#define  DLL_IMPORT   extern "C" __declspec( dllimport )

// Can't use extern "C" when DLL exporting a class
#define  DLL_CLASS_EXPORT   __declspec( dllexport ) 
#define  DLL_CLASS_IMPORT   __declspec( dllimport )

// Can't use extern "C" when DLL exporting a global
#define  DLL_GLOBAL_EXPORT   extern __declspec( dllexport ) 
#define  DLL_GLOBAL_IMPORT   extern __declspec( dllimport )

#define  stdcall    __stdcall
#define  fastcall   __fastcall

// Hints to the optimizer
// see http://msdn2.microsoft.com/en-us/library/1b3fsfxw(VS.80).aspx
#ifdef _MSC_VER
    #define HINT(THE_HINT)    __assume((THE_HINT))
#else
    #define HINT(THE_HINT)    0
#endif

// Marks the codepath from here until the next branch entry point as unreachable,
// and asserts if any attempt is made to execute it.
#define UNREACHABLE() { ASSERT(0); HINT(0); }

/*
Methods to invoke the constructor, copy constructor, and destructor
*/

template<class T> 
inline void Construct( T* ptr)
{
    new(ptr) T;
}

template<class T> 
inline void CopyConstruct(T* ptr, T const& src)
{
    new(ptr) T(src);
}

template <class T> 
inline void Destruct(T* ptr)
{
    ptr->~T();
#ifdef _DEBUG
    memset(ptr, 0xDD, sizeof(T));
#endif
}


/*
 OUTER()

 A platform-independent way for a contained class to get a pointer to its
 owner. If you know a class is exclusively used in the context of some
 "outer" class, this is a much more space efficient way to get at the outer
 class than having the inner class store a pointer to it.

    class Outer
    {
        class Inner // Note: this does not need to be a nested class to work
        {
            void PrintAddressOfOuter()
            {
                printf( "Outer is at 0x%x\n", OUTER( Outer, inner ) );
            }
        };
        
        Inner inner;
        friend class Inner;
    };
*/

#define OUTER(OuterType, OuterMember) \
   ((OuterType*)((char*)this - offsetof(OuterType, OuterMember)))

#if defined(P3D_WINDOWS)
#include "Common/Windows/Includes.h"
#endif

// tinyxml
#include "TinyXML/tinyxml.h"

// SDL
#include "SDL/include/SDL.h"