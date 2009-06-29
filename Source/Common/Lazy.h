#pragma once

namespace P3D
{
    template <class OwnerType, class T>
    class Lazy
    {
        typedef void (OwnerType::*RecalculateMethod)(T&);
    public:
        Lazy(OwnerType* owner, RecalculateMethod method) :
          _owner(owner), _dirty(true), _method(method)
        { }

        void Invalidate() { _dirty = true; }
        bool IsDirty() const { return _dirty; }

        T& operator()()
        {
            Update();
            return _t;
        }

        const T& operator()() const
        {
            Update();
            return _t;
        }

        void Update() const
        {
            if (_dirty)
            {
                (const_cast<OwnerType*>(_owner)->*_method)(_t);
                _dirty = false;
            }
        }

        void Set(const T& t)
        {
            _t = t;
            _dirty = false;
        }

    private:
        OwnerType* _owner;
        RecalculateMethod _method;
 
        mutable bool _dirty;
        mutable T _t;
    };
}