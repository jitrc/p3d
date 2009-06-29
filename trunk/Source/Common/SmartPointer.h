#pragma once

namespace P3D
{
    template <class T>
    class _NoAddRefOnObject : public T
    {
    private:
        virtual int AddRef ();
        virtual int Release ();
    };

    /*
    Smart pointer class.
    (c) some where....
    T should support AddRef and Release
    */
    template <class T>
    class SmartPointer
    {
    protected:
        T* p;

    public:
        SmartPointer() throw()
        {
            p = NULL;
        }
        SmartPointer(int nNull) throw()
        {
            ASSERT(nNull == 0);
            (void)nNull;
            p = NULL;
        }

        SmartPointer(T* lp) throw()
        {
            p = lp;
            if (p != NULL) p->AddRef();
        }

        SmartPointer(const SmartPointer<T>& lp) throw()
        {
            p = lp.p;
            if (p != NULL) p->AddRef();
        }

        ~SmartPointer() throw()
        {
            if (p) p->Release();
        }

        operator T*() const throw()
        {
            return p;
        }

        T& operator*() const
        {
            return *p;
        }

        //The assert on operator& usually indicates a bug.  If this is really
        //what is needed, however, take the address of the p member explicitly.
        T** operator&() throw()
        {
            //ASSERT(p==NULL);
            return &p;
        }

        _NoAddRefOnObject<T>* operator->() const throw()
        {
            ASSERT(p!=NULL);
            return (_NoAddRefOnObject<T>*)p;
        }

        bool operator!() const throw()
        {
            return (p == NULL);
        }

        bool operator<(T* pT) const throw()
        {
            return p < pT;
        }

        bool operator!=(T* pT) const
        {
            return !operator==(pT);
        }

        bool operator==(T* pT) const throw()
        {
            return p == pT;
        }

        T* operator=(T* lp) throw()
        {
            if (*this!=lp)
            {
                if (lp) lp->AddRef();
                if (p) p->Release();
                p = lp;
            }
            return *this;
        }

        T* operator=(const SmartPointer<T>& lp) throw()
        {
            if (*this!=lp)
            {
                if (lp.p) lp.p->AddRef();
                if (p) p->Release();
                p = lp.p;
            }
            return *this;
        }

        // Release the interface and set to NULL
        void Release() throw()
        {
            T* pTemp = p;
            if (pTemp)
            {
                p = NULL;
                pTemp->Release();
            }
        }
        // Attach to an existing interface (does not AddRef)
        void Attach(T* p2) throw()
        {
            if (p) p->Release();
            p = p2;
        }

        // Detach the interface (does not Release)
        T* Detach() throw()
        {
            T* pt = p;
            p = NULL;
            return pt;
        }
        void CopyTo(T** ppT) throw()
        {
            ASSERT(ppT != NULL);
            if (ppT == NULL) return;
            *ppT = p;
            if (p) p->AddRef();
        }
    };
}