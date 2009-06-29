#pragma once

namespace P3D
{
    template<class T>
    struct TypeTraits
    {
        typedef T Type;
        typedef T* PointerType;
        typedef T& RefType;
        typedef const T& ParamType;
    };

    template<class T>
    struct TypeTraits<T&>
    {
        typedef T& Type;
        typedef T* PointerType;
        typedef T& RefType;
        typedef T& ParamType;
    };

    template<class T>
    struct TypeTraits<T*>
    {
        typedef T* Type;
        typedef T** PointerType;
        typedef T*& RefType;
        typedef T* ParamType;
    };

    /*
    Checks at compile time is there static cast from T to U.
    */
    template<class T, class U>
    class CanCast
    {
        typedef char Small;
        typedef struct { char dummy[2]; } Big;

        static T makeT();
        static Small convTest(U);
        static Big convTest(...);
    public:
        enum
        {
            Value = (sizeof(convTest(makeT())) == sizeof(Small))
        };
    };

    namespace Internal
    {
        template<class T, bool canCastToObj>
        struct AddRefIfNeededImpl;

        template<class T>
        struct AddRefIfNeededImpl<T, true>
        {
            int operator()(typename TypeTraits<T>::ParamType t)
            {
                if (t) return t->AddRef(); else return 0;
            }
        };

        template<class T>
        struct AddRefIfNeededImpl<T, false>
        {
            int operator()(typename TypeTraits<T>::ParamType t)
            {
                return 0;
            }
        };

        template<class T, bool canCastToObj>
        struct ReleaseIfNeededImpl;

        template<class T>
        struct ReleaseIfNeededImpl<T, true>
        {
            int operator()(typename TypeTraits<T>::ParamType t)
            {
                if (t) return t->Release(); else return 0;
            }
        };

        template<class T>
        struct ReleaseIfNeededImpl<T, false>
        {
            int operator()(typename TypeTraits<T>::ParamType t)
            {
                return 0;
            }
        };
    }

    template<class T>
    int AddRefIfNeeded(T t) { return Internal::AddRefIfNeededImpl<T, CanCast<T, Object*>::Value >()(t); }

    template<class T>
    int ReleaseIfNeeded(T t) { return Internal::ReleaseIfNeededImpl<T, CanCast<T, Object*>::Value >()(t); }

    template<class Map, class Key, class T>
    const T& GetFromMap(const Map& map, const Key key, const T& def)
    {
        Map::const_iterator it = map.find(key);
        if (it == map.end()) return def;
        else return it->second;
    }

    template<bool cond, class T, class E>
    struct IfThenElse;

    template<class T, class E>
    struct IfThenElse<true, T, E>
    {
        typedef typename T Value;
    };

    template<class T, class E>
    struct IfThenElse<false, T, E>
    {
        typedef typename E Value;
    };
}