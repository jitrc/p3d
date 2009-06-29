#pragma once

#include "LogMessage.h"
#include "UtilFunctions.h"
#include "Exceptions.h"

namespace P3D
{
    class Logger;

    class LogStream
    {
        friend class Logger;
    public:
        inline LogStream& operator<<(const wchar* ch) { _buffer += ch; return *this; }
        inline LogStream& operator<<(const String& ch) { _buffer += ch; return *this; }
        inline LogStream& operator<<(const char* ch) { _buffer += String(ToUTF16(ch)); return *this; }
        inline LogStream& operator<<(const std::string& ch) { _buffer += ToUTF16(ch); return *this; }
        inline LogStream& operator<<(int i)  {  wchar buf[256]; _buffer += _itow(i, buf, 10); return *this; }
        inline LogStream& operator<<(unsigned int i)  {  wchar buf[256]; _buffer += _itow(i, buf, 10); return *this; }
        inline LogStream& operator<<(double i) { char buf[_CVTBUFSIZE]; *this << _gcvt(i, 4, buf); return *this; }
        inline LogStream& operator<<(const Exception& exc) { *this << exc.GetFullDescription(); return *this; }
        inline LogStream& operator<<(void* ptr)
        { 
            wchar buf1[32];
            _itow(reinterpret_cast<size_t>(ptr), buf1, 16);
            int len = wcslen(buf1);

            wchar buf2[32];
            buf2[0] = L'0'; buf2[1] = L'x';
            int i = 2;
            for (unsigned int j = 0; j < sizeof(void*)*2 - len; j++)
            {
                buf2[i] = L'0';
                i++;
            }
            buf2[i] = 0;
            _buffer += buf2;
            _buffer += buf1;
            return *this;
        }
        

        ~LogStream();

    private:
        LogStream(Logger* owner, LogLevel level) : _owner(owner), _level(level) { }

        Logger* _owner;
        LogLevel _level;
        String _buffer;
    };
}