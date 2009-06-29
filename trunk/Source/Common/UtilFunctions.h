#pragma once

class TiXmlElement;

namespace P3D
{
    /*
    Converts UTF-8 string to UTF-16 string.
    */
    const String ToUTF16(const char* ch);

    /*
    Converts UTF-8 string to UTF-16 string.
    */
    inline const String ToUTF16(const std::string& str) { return ToUTF16(str.c_str()); }

    /*
    Converts UTF-16 string to UTF-8 string.
    */
    const std::string ToUTF8(const wchar* ch);

    /*
    Converts UTF-16 string to UTF-8 string.
    */
    inline const std::string ToUTF8(const String& str) { return ToUTF8(str.c_str()); }

    template<class T>
    T random(T maximum)
    {
        int randomValue = rand();
        return (T)((randomValue*maximum) / RAND_MAX);
    }

    template<class T>
    T random(T min, T max)
    {
        return min + random(max-min);
    }

    template<typename Char>
    uint32 GetStringHash(const Char* string)
    {
        uint32 hash;
        uint32 c;

        hash = 0;
        while (*string)
        {
            c = *string++;
            hash = 127 * hash + c;
        }

        hash = (hash >> 20) ^ (hash >> 10) ^ hash;
        return hash;
    }

    /*
    Return random string that can be used as ID. It starts with alphabetical symbol or '_' and 
    consists only of alpha-numeric characters.
    */
    const std::string RandomIDString(int length, bool lowercase = false);

    /*
    Attach system debugger to the calling process.
    Will wait till timeout value for debugger to attach before returning from the function.
    Return true in case debugger attached.
    */
    bool AttachDebugger(int timeout = INFINITE);

    /*
    Is debugger attached to the process?
    */
    bool IsUnderDebugger();

    /*
    Opens text console window for process.
    */
    void OpenConsole(bool attachToParent = true);

    /*
    Encode buffer in base64 encoding.
    */
    const std::string Encode64(const std::string& input);

    /*
    Decode buffer from base64 encoding.
    */
    const std::string Decode64(const std::string& input);

    /*
    Calculate CRC-32-IEEE 802.3 sum.
    */
    const int32 CRC32(const void* buf, unsigned long len);

    /*
    Splits string i.e "word1, word2" -> "word1", "word2".
    Template parameter T should specify type that support push_back(String s);
    */
    template<class T>
    uint SplitString(T& output, const wchar* string, wchar separator, bool stripSpaces = true)
    {
        const wchar* ptr = string;
        String subStr;
        uint count = 0;
        while (*ptr)
        {
            if (stripSpaces && (*ptr == ' ' || *ptr == '\t')) { ptr++; continue; }
            if (*ptr == separator)
            {
                output.push_back(subStr);
                count++;
                subStr.clear();
            } else
                subStr+=*ptr;
            ptr++;
        };
        if (!subStr.empty())
        {
            output.push_back(subStr);
            count++;
        }
        return count;
    }

    /*
    Reads boolean attribute from XML element.
    */
    bool XmlReadBool(const TiXmlElement* element, const char* attrName, bool def);
}