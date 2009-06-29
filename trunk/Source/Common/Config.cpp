#include "Includes.h"
#include "Config.h"
#include "Logger.h"

namespace P3D
{
    DEFINE_SINGLETON(Config);
    Logger Config::logger(L"System.Config");

    Config::Config(const String& conf)
    {
        AutoLock lock(_lock);

        if (conf.empty())
            _filePath = L"config.xml";
        else
            _filePath = conf;

        Reload();
    }

    Config::~Config()
    {
    }

    void Config::Reload()
    {
        AutoLock lock(_lock);

        FILE* file = _wfopen(_filePath.c_str(), L"rb");
        if (file == NULL) return;
        _config.Clear();
        _config.LoadFile(file);
        fclose(file);
    }

    const TiXmlElement* Config::GetSection(const char* section)
    {
        AutoLock lock(_lock);
        TiXmlElement* root = _config.RootElement();
        if (root == NULL) return NULL;
        return root->FirstChildElement(section);
    }

    const std::string Config::ReadAnsiString(const std::string& section, 
        const std::string& name, const std::string& def)
    {
        const TiXmlElement* tag = GetSection(section.c_str());
        if (tag == NULL) return def;
        const char* attr = tag->Attribute(name.c_str());
        if (attr == NULL) return def;
        else return attr;
    }

    const String Config::ReadWideString(const std::string& section, 
        const std::string& name, const String& def)
    {
        const TiXmlElement* tag = GetSection(section.c_str());
        if (tag == NULL) return def;
        const char* attr = tag->Attribute(name.c_str());
        if (attr == NULL) return def;
        else return ToUTF16(attr);
    }

    int Config::ReadInt(const std::string& section, 
        const std::string& name, int def)
    {
        const TiXmlElement* tag = GetSection(section.c_str());
        if (tag == NULL) return def;
        int value = def;
        tag->QueryIntAttribute(name.c_str(), &value);
        return value;
    }

    bool Config::ReadBool(const std::string& section, const std::string& name, bool def)
    {
        const TiXmlElement* tag = GetSection(section.c_str());
        if (tag == NULL) return def;
        return XmlReadBool(tag, name.c_str(), def);
    }
}