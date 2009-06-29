#pragma once

#include "Synchronization.h"

namespace P3D
{
    /*
    Object that can hold orbitary string -> void* mapping.
    */
    class ObjectWithTags
    {
    public:
        /*
        Disposes all tags.
        */
        virtual ~ObjectWithTags()
        {
            AutoLock lock(_lock);
            for (TagIterator it = _tags.begin(); it != _tags.end(); ++it)
                DisposeTag(it->second);
            _tags.clear();
        }

        /*
        Adds new tag. Rewrites previous one with the same name.
        */
        inline void AddTag(const char* key, void* value, TagDisposeFunction disposeFunction = NULL)
        {
            AutoLock lock(_lock);
            TagIterator it = _tags.find(key);
            if (it != _tags.end())
            {
                DisposeTag(it->second);
                if (value == NULL)
                    _tags.erase(it);
                else
                {
                    it->second.Data = value;
                    it->second.DisposeFunc = disposeFunction;
                }
            } else
            {
                Tag tag;
                tag.Data = value;
                tag.DisposeFunc = disposeFunction;
                _tags[key] = tag;
            }
        }

        /*
        Remove tag if it exists.
        */
        inline void RemoveTag(const char* key) { AddTag(key, NULL); }

        /*
        Remove all tags.
        */
        inline void RemoveAllTags()
        {
            AutoLock lock(_lock);
            for (TagIterator it = _tags.begin(); it != _tags.end(); ++it)
                DisposeTag(it->second);
            _tags.clear();
        }

        /*
        Return tag as void*.
        */
        void* GetTag(const char* key) const
        {
            return GetTagImpl(key);
        }

        /*
        Return tag or NULL. Makes typecast.
        */
        template<class T>
        T* GetTagWithType(const char* key) const
        {
            return static_cast<T*>(GetTagImpl(key));
        }

    private:
        struct Tag
        {
            void* Data;
            TagDisposeFunction DisposeFunc;
        };

        typedef std::map<std::string, Tag> TagsMap;
        typedef TagsMap::const_iterator TagConstIterator;
        typedef TagsMap::iterator TagIterator;

        TagsMap _tags;

    private:
        inline void DisposeTag(Tag& tag)
        {
            if (tag.Data != NULL && tag.DisposeFunc != NULL)
                tag.DisposeFunc(tag.Data);
            tag.Data = tag.DisposeFunc = NULL;
        }

        inline void* GetTagImpl(const char* key) const
        {
            AutoLock lock(_lock);
            TagConstIterator it = _tags.find(key);
            if (it == _tags.end()) return NULL;
            else return it->second.Data;
        }

        mutable Lock _lock;
    };
}