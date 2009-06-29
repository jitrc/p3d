#include "Includes.h"
#include "ObjectPool.h"

namespace P3D
{
    ReusableObject::ReusableObject() : _owner(NULL)
    { }

    int ReusableObject::AddRef()
    {
        int count = Object::AddRef();
        if (_owner)
            return count - 1;
        else
            return count;
    }

    int ReusableObject::Release()
    {
        int count = Object::Release();
        if (count == 0 || !_owner)
            return count;
        if (count == 1)
        {
            _owner->Collect(this);
            return 0;
        } else
            return count - 1;
    }

    ReusableObject* ObjectPoolBase::CreateObject()
    {
        ReusableObject* obj = NULL;
        auto_lock(_lock)
        {
            if (!_free.empty())
            {
                obj = _free.back();
                _free.pop_back();
            }
        }
        if (obj == NULL)
        {
            obj = CreateObjectInstance();
            obj->SetOwner(this);
        }
        obj->ReinitializeReusable();
        obj->AddRef();
        return obj;
    }

    void ObjectPoolBase::Collect(ReusableObject* obj)
    {
        obj->DeinitializeReusable();
        auto_lock(_lock)
        {
            _free.push_back(obj);
        }
    }

    ObjectPoolBase::~ObjectPoolBase()
    {
        Clear();
    }

    void ObjectPoolBase::Clear()
    {
        auto_lock(_lock)
        {
            for (unsigned int i = 0; i < _free.size(); i++)
            {
                _free[i]->SetOwner(NULL);
                _free[i]->Release();
            }
            _free.clear();
        }
    }
}