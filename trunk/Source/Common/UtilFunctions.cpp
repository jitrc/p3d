#include "Includes.h"
#include "UtilFunctions.h"

namespace P3D
{    
    const String ToUTF16(const char* ch)
    {
        if (ch == NULL || ch[0] == 0) return L"";

#ifdef P3D_WINDOWS
        int requiredSize = MultiByteToWideChar(CP_UTF8, 0, ch, -1, NULL, 0);
        String res;
        res.reserve(requiredSize);
        res.resize(requiredSize - 1);
        MultiByteToWideChar(CP_UTF8, 0, ch, -1, (wchar*)res.data(), requiredSize);
        return res;
#else
#error Not implemented yet.
#endif
    }

    const std::string ToUTF8(const wchar* ch)
    {
        if (ch == NULL || ch[0] == 0) return "";

#ifdef P3D_WINDOWS
        int requiredSize = WideCharToMultiByte(CP_UTF8, 0, ch, -1, NULL, 0, NULL, NULL);
        std::string res;
        res.reserve(requiredSize);
        res.resize(requiredSize - 1);
        WideCharToMultiByte(CP_UTF8, 0, ch, -1, (char*)res.data(), requiredSize, NULL, NULL);
        return res;
#else
#error Not implemented yet.
#endif
    }

    const std::string RandomIDString(int length, bool lowercase)
    {
        std::string res;
        res.reserve(length);

        for (int i = 0; i < length; i++)
        {
            int rand = random(100);
            if (rand < 20 && i != 0)
                res += ('0' + random(0, 10));
            else if (rand < 60 || lowercase)
                res += ('a' + random(0, 'z'-'a'+1));
            else
                res += ('A' + random(0, 'Z'-'A'+1));
        }

        return res;
    }

#if defined(P3D_WINDOWS)
    static bool ReadRegString(HKEY root, const wchar_t* subkey, 
        const wchar_t* value, wchar_t* buffer, int* len)
    {
        HKEY key = NULL;
        DWORD res = RegOpenKeyEx(root, subkey, 0, KEY_READ, &key);
        if (ERROR_SUCCESS != res || key == NULL)
            return false;

        DWORD type = 0;
        DWORD size = *len * sizeof(wchar_t);

        res = RegQueryValueEx(key, value, NULL, &type,
            reinterpret_cast<BYTE*>(buffer), &size);

        if (ERROR_SUCCESS == res && size != 0 && type == REG_SZ) 
        {
            buffer[*len - 1] = 0;
            *len = lstrlen(buffer);
            RegCloseKey(key);
            return true;
        }
        RegCloseKey(key);
        return false;
    }
#endif

    bool AttachDebugger(int timeout)
    {
#if defined(P3D_WINDOWS)
        if (IsUnderDebugger()) return true;
        DWORD processId = GetCurrentProcessId();
        wchar_t debuggerCmd[1024*4];
        int len = sizeof(debuggerCmd) / sizeof(wchar_t);
        if (!ReadRegString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AeDebug", 
            L"Debugger", debuggerCmd, &len))
                return false;

        const int outBufLen = 1024*4;
        wchar_t output[outBufLen] = {0};
        int i = 0;

        // replace %ld by pid
        wchar_t* cur = debuggerCmd;
        while (*cur && i < outBufLen-1)
        {
            if (*cur == L'%' && *(cur+1)==L'l' && *(cur+2)==L'd')
            {
                wsprintf(&output[i], L"%d", processId);
                i = lstrlen(output);
                cur += 2;
            } else
            {
                output[i] = *cur;
                i++;
            }
            cur++;
        }
        output[i] = 0;

        STARTUPINFO startupInfo = {0};
        startupInfo.cb = sizeof(startupInfo);
        PROCESS_INFORMATION processInfo = {0};

        if (!CreateProcess(NULL, output, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo))
            return false;

        CloseHandle(processInfo.hThread);
        WaitForInputIdle(processInfo.hProcess, 10000);
        CloseHandle(processInfo.hProcess);

        // wait for debugger to attach
        DWORD startTime = GetTickCount() - 1;
        while (true)
        {
            DWORD time = GetTickCount() - startTime;
            if (timeout >= 0 && time > (DWORD)timeout)
                return false;
            if (IsUnderDebugger())
                return true;
            Sleep(100);
        }
#else
        return false;
#endif
    }

    bool IsUnderDebugger()
    {
#if defined(P3D_WINDOWS)
        return ::IsDebuggerPresent() != 0;
#else
        return false;
#endif
    }

    void OpenConsole(bool attachToParent)
    {
#ifdef P3D_WINDOWS
        FreeConsole();

        if (!attachToParent)
            AllocConsole();
        else
        {
            if (AttachConsole(ATTACH_PARENT_PROCESS) == 0)
                AllocConsole();
        }

        freopen("CONOUT$", "a", stderr);
        freopen("CONOUT$", "a", stdout);
#endif
    }


    // base64 encoding\decoding is taken from gloox
    // Copyright (c) 2005-2008 by Jakob Schroeter <js@camaya.net>

    static const std::string alphabet64( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" );
    static const char pad = '=';
    static const char np  = (char)std::string::npos;
    static char table64vals[] =
    {
        62, np, np, np, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, np, np, np, np, np,
        np, np,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23, 24, 25, np, np, np, np, np, np, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
    };

    inline char table64( unsigned char c )
    {
        return ( c < 43 || c > 122 ) ? np : table64vals[c-43];
    }

    const std::string Encode64( const std::string& input )
    {
        std::string encoded;
        char c;
        const std::string::size_type length = input.length();

        encoded.reserve( length * 2 );

        for( std::string::size_type i = 0; i < length; ++i )
        {
            c = ( input[i] >> 2 ) & 0x3f;
            encoded += alphabet64[c];

            c = ( input[i] << 4 ) & 0x3f;
            if( ++i < length )
                c |= ( ( input[i] >> 4 ) & 0x0f );
            encoded += alphabet64[c];

            if( i < length )
            {
                c = ( input[i] << 2 ) & 0x3c;
                if( ++i < length )
                    c |= ( input[i] >> 6 ) & 0x03;
                encoded += alphabet64[c];
            }
            else
            {
                ++i;
                encoded += pad;
            }

            if( i < length )
            {
                c = input[i] & 0x3f;
                encoded += alphabet64[c];
            }
            else
            {
                encoded += pad;
            }
        }

        return encoded;
    }

    const std::string Decode64( const std::string& input )
    {
        char c, d;
        const std::string::size_type length = input.length();
        std::string decoded;

        decoded.reserve( length );

        for( std::string::size_type i = 0; i < length; ++i )
        {
            c = table64(input[i]);
            ++i;
            d = table64(input[i]);
            c = ( c << 2 ) | ( ( d >> 4 ) & 0x3 );
            decoded += c;
            if( ++i < length )
            {
                c = input[i];
                if( pad == c )
                    break;

                c = table64(input[i]);
                d = ( ( d << 4 ) & 0xf0 ) | ( ( c >> 2 ) & 0xf );
                decoded += d;
            }

            if( ++i < length )
            {
                d = input[i];
                if( pad == d )
                    break;

                d = table64(input[i]);
                c = ( ( c << 6 ) & 0xc0 ) | d;
                decoded += c;
            }
        }

        return decoded;
    }

    // http://ru.wikipedia.org/wiki/%D0%A6%D0%B8%D0%BA%D0%BB%D0%B8%D1%87%D0%B5%D1%81%D0%BA%D0%B8%D0%B9_%D0%B8%D0%B7%D0%B1%D1%8B%D1%82%D0%BE%D1%87%D0%BD%D1%8B%D0%B9_%D0%BA%D0%BE%D0%B4
    const int32 CRC32(const void* ptr, unsigned long len)
    {
        unsigned char *buf = (unsigned char *)ptr;
        unsigned long crc;
        static unsigned long crc_table[256];
        static bool tableInitialized = false;
     
        if (!tableInitialized)
        {
            for (int i = 0; i < 256; i++)
            {
                crc = i;
                for (int j = 0; j < 8; j++)
                    crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
         
                crc_table[i] = crc;
            };
            tableInitialized = true;
        }
     
        crc = 0xFFFFFFFFUL;
     
        while (len--) 
            crc = crc_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);
     
        return crc ^ 0xFFFFFFFFUL;
    }

    bool XmlReadBool(const TiXmlElement* element, const char* attrName, bool def)
    {
        if (element == NULL) return def;
        const char* attr = element->Attribute(attrName);
        if (attr == NULL) return def;
        if (strcmpi(attr, "true") == 0) return true;
        if (strcmpi(attr, "false") == 0) return false;
        return def;
    }
}