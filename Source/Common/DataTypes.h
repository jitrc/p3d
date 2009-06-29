#pragma once

namespace P3D
{
    typedef unsigned char       byte;
    typedef signed char         sbyte;
    typedef unsigned short      ushort;
    typedef unsigned __int16    uint16;
    typedef __int16             int16;
    typedef unsigned int        uint;
    typedef unsigned __int32    uint32;
    typedef __int32             int32;
    typedef unsigned long       ulong;
    typedef unsigned __int64    uint64;
    typedef __int64             int64;
    typedef wchar_t             wchar;
    typedef float               float32;
    typedef double              float64;

    typedef std::basic_string<wchar, std::char_traits<wchar>, std::allocator<wchar> > String;
    typedef void (*TagDisposeFunction)(void* tag);
}