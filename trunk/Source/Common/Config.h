#pragma once

#include "Logger.h"
#include "Singleton.h"
#include "Synchronization.h"

namespace P3D
{
    class Config :
        private Singleton<Config>
    {
        friend class Singleton<Config>;
        static Logger logger;

        Config(const String& config = L"");
        ~Config();

    public:
        // Return class instance.
        static Config& GetInstance()
        {
            return Singleton<Config>::GetInstance();
        }

        const TiXmlElement* GetSection(const char* section);

        void Reload();

        /*
        Reads attribute from config section.
        */
        const std::string ReadAnsiString(const std::string& section, const std::string& name, const std::string& def = "");
        const String      ReadWideString(const std::string& section, const std::string& name, const String& def = L"");
        int               ReadInt(const std::string& section, const std::string& name, int def = 0);
        bool              ReadBool(const std::string& section, const std::string& name, bool def = false);

    private:
        TiXmlDocument _config;
        Lock _lock;
        String _filePath;
    };
}