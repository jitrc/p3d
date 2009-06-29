#pragma once

#include "Command.h"
#include "TemplateTricks.h"

namespace P3D
{
    namespace Internal
    {
        template<class Obj, class Method>
        struct Command0 : public Command
        {
            Command0(Obj* o, Method m) : obj(o), method(m) 
            {
                AddRefIfNeeded(obj);
            }
            virtual ~Command0()
            {
                ReleaseIfNeeded(obj);
            }
            override void Execute(ExecutionContext* cmdHandler) { ((*((Obj*)obj)).*(method))(); }
            Method method;
            Obj* obj;
        };

        template<class Obj, class Method, class Arg1>
        struct Command1 : public Command
        {
            Command1(Obj* o, Method m, Arg1 a1) : obj(o), method(m), A1(a1)
            {
                AddRefIfNeeded(obj);
                AddRefIfNeeded(A1);
            }
            virtual ~Command1()
            {
                ReleaseIfNeeded(obj);
                ReleaseIfNeeded(A1);
            }
            override void Execute(ExecutionContext* cmdHandler) { ((*((Obj*)obj)).*(method))(A1); }
            Method method;
            Obj* obj;
            Arg1 A1;
        };

        template<class Obj, class Method, class Arg1, class Arg2>
        struct Command2 : public Command
        {
            Command2(Obj* o, Method m, Arg1 a1, Arg2 a2) : obj(o), method(m), A1(a1), A2(a2)
            {
                AddRefIfNeeded(obj);
                AddRefIfNeeded(A1);
                AddRefIfNeeded(A2);
            }
            virtual ~Command2()
            {
                ReleaseIfNeeded(obj);
                ReleaseIfNeeded(A1);
                ReleaseIfNeeded(A2);
            }
            override void Execute(ExecutionContext* cmdHandler) { ((*((Obj*)obj)).*(method))(A1, A2); }
            Method method;
            Obj* obj;
            Arg1 A1;
            Arg2 A2;
        };

        template<class Obj, class Method, class Arg1, class Arg2, class Arg3>
        struct Command3 : public Command
        {
            Command3(Obj* o, Method m, Arg1 a1, Arg2 a2, Arg3 a3) : obj(o), method(m), A1(a1), A2(a2), A3(a3)
            {
                AddRefIfNeeded(obj);
                AddRefIfNeeded(A1);
                AddRefIfNeeded(A2);
                AddRefIfNeeded(A3);
            }
            virtual ~Command3()
            {
                ReleaseIfNeeded(obj);
                ReleaseIfNeeded(A1);
                ReleaseIfNeeded(A2);
                ReleaseIfNeeded(A3);
            }
            override void Execute(ExecutionContext* cmdHandler) { ((*((Obj*)obj)).*(method))(A1, A2, A3); }
            Method method;
            Obj* obj;
            Arg1 A1;
            Arg2 A2;
            Arg3 A3;
        };

        template<class Obj, class Method, class Arg1, class Arg2, class Arg3, class Arg4>
        struct Command4 : public Command
        {
            Command4(Obj* o, Method m, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4) : obj(o), method(m), A1(a1), A2(a2), A3(a3), A4(a4)
            {
                AddRefIfNeeded(obj);
                AddRefIfNeeded(A1);
                AddRefIfNeeded(A2);
                AddRefIfNeeded(A3);
                AddRefIfNeeded(A4);
            }
            virtual ~Command4()
            {
                ReleaseIfNeeded(obj);
                ReleaseIfNeeded(A1);
                ReleaseIfNeeded(A2);
                ReleaseIfNeeded(A3);
                ReleaseIfNeeded(A4);
            }
            override void Execute(ExecutionContext* cmdHandler) { ((*((Obj*)obj)).*(method))(A1, A2, A3, A4); }
            Method method;
            Obj* obj;
            Arg1 A1;
            Arg2 A2;
            Arg3 A3;
            Arg4 A4;
        };

        template<class Obj, class Method, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
        struct Command5 : public Command
        {
            Command5(Obj* o, Method m, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5) : obj(o), method(m), A1(a1), A2(a2), A3(a3), A4(a4), A5(a5)
            {
                AddRefIfNeeded(obj);
                AddRefIfNeeded(A1);
                AddRefIfNeeded(A2);
                AddRefIfNeeded(A3);
                AddRefIfNeeded(A4);
                AddRefIfNeeded(A5);
            }
            virtual ~Command5()
            {
                ReleaseIfNeeded(obj);
                ReleaseIfNeeded(A1);
                ReleaseIfNeeded(A2);
                ReleaseIfNeeded(A3);
                ReleaseIfNeeded(A4);
                ReleaseIfNeeded(A5);
            }
            override void Execute(ExecutionContext* cmdHandler) { ((*((Obj*)obj)).*(method))(A1, A2, A3, A4, A5); }
            Method method;
            Obj* obj;
            Arg1 A1;
            Arg2 A2;
            Arg3 A3;
            Arg4 A4;
            Arg5 A5;
        };

        template<class Obj, class Method, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6>
        struct Command6 : public Command
        {
            Command6(Obj* o, Method m, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6) : obj(o), method(m), A1(a1), A2(a2), A3(a3), A4(a4), A5(a5), A6(a6)
            {
                AddRefIfNeeded(obj);
                AddRefIfNeeded(A1);
                AddRefIfNeeded(A2);
                AddRefIfNeeded(A3);
                AddRefIfNeeded(A4);
                AddRefIfNeeded(A5);
                AddRefIfNeeded(A6);
            }
            virtual ~Command6()
            {
                ReleaseIfNeeded(obj);
                ReleaseIfNeeded(A1);
                ReleaseIfNeeded(A2);
                ReleaseIfNeeded(A3);
                ReleaseIfNeeded(A4);
                ReleaseIfNeeded(A5);
                ReleaseIfNeeded(A6);
            }
            override void Execute(ExecutionContext* cmdHandler) { ((*((Obj*)obj)).*(method))(A1, A2, A3, A4, A5, A6); }
            Method method;
            Obj* obj;
            Arg1 A1;
            Arg2 A2;
            Arg3 A3;
            Arg4 A4;
            Arg5 A5;
            Arg6 A6;
        };

        template<class Obj, class Method, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7>
        struct Command7 : public Command
        {
            Command7(Obj* o, Method m, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6, Arg7 a7) : obj(o), method(m), A1(a1), A2(a2), A3(a3), A4(a4), A5(a5), A6(a6), A7(a7)
            {
                AddRefIfNeeded(obj);
                AddRefIfNeeded(A1);
                AddRefIfNeeded(A2);
                AddRefIfNeeded(A3);
                AddRefIfNeeded(A4);
                AddRefIfNeeded(A5);
                AddRefIfNeeded(A6);
                AddRefIfNeeded(A7);
            }
            virtual ~Command7()
            {
                ReleaseIfNeeded(obj);
                ReleaseIfNeeded(A1);
                ReleaseIfNeeded(A2);
                ReleaseIfNeeded(A3);
                ReleaseIfNeeded(A4);
                ReleaseIfNeeded(A5);
                ReleaseIfNeeded(A6);
                ReleaseIfNeeded(A7);
            }
            override void Execute(ExecutionContext* cmdHandler) { ((*((Obj*)obj)).*(method))(A1, A2, A3, A4, A5, A6, A7); }
            Method method;
            Obj* obj;
            Arg1 A1;
            Arg2 A2;
            Arg3 A3;
            Arg4 A4;
            Arg5 A5;
            Arg6 A6;
            Arg7 A7;
        };

        template<class Obj, class Method, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8>
        struct Command8 : public Command
        {
            Command8(Obj* o, Method m, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6, Arg7 a7, Arg8 a8) : obj(o), method(m), A1(a1), A2(a2), A3(a3), A4(a4), A5(a5), A6(a6), A7(a7), A8(a8)
            {
                AddRefIfNeeded(obj);
                AddRefIfNeeded(A1);
                AddRefIfNeeded(A2);
                AddRefIfNeeded(A3);
                AddRefIfNeeded(A4);
                AddRefIfNeeded(A5);
                AddRefIfNeeded(A6);
                AddRefIfNeeded(A7);
                AddRefIfNeeded(A8);
            }
            virtual ~Command8()
            {
                ReleaseIfNeeded(obj);
                ReleaseIfNeeded(A1);
                ReleaseIfNeeded(A2);
                ReleaseIfNeeded(A3);
                ReleaseIfNeeded(A4);
                ReleaseIfNeeded(A5);
                ReleaseIfNeeded(A6);
                ReleaseIfNeeded(A7);
                ReleaseIfNeeded(A8);
            }
            override void Execute(ExecutionContext* cmdHandler) { ((*((Obj*)obj)).*(method))(A1, A2, A3, A4, A5, A6, A7, A8); }
            Method method;
            Obj* obj;
            Arg1 A1;
            Arg2 A2;
            Arg3 A3;
            Arg4 A4;
            Arg5 A5;
            Arg6 A6;
            Arg7 A7;
            Arg8 A8;
        };
    }


    template<class Obj, class Method>
    Command* MC(Obj* obj, Method m) { return new Internal::Command0<Obj, Method>(obj, m); }
    template<class Obj, class Method, class Arg1>
    Command* MC(Obj* obj, Method m, Arg1 a1) { return new Internal::Command1<Obj, Method, Arg1>(obj, m, a1); }
    template<class Obj, class Method, class Arg1, class Arg2>
    Command* MC(Obj* obj, Method m, Arg1 a1, Arg2 a2) { return new Internal::Command2<Obj, Method, Arg1, Arg2>(obj, m, a1, a2); }
    template<class Obj, class Method, class Arg1, class Arg2, class Arg3>
    Command* MC(Obj* obj, Method m, Arg1 a1, Arg2 a2, Arg3 a3) { return new Internal::Command3<Obj, Method, Arg1, Arg2, Arg3>(obj, m, a1, a2, a3); }
    template<class Obj, class Method, class Arg1, class Arg2, class Arg3, class Arg4>
    Command* MC(Obj* obj, Method m, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4) { return new Internal::Command4<Obj, Method, Arg1, Arg2, Arg3, Arg4>(obj, m, a1, a2, a3, a4); }
    template<class Obj, class Method, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
    Command* MC(Obj* obj, Method m, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5) { return new Internal::Command5<Obj, Method, Arg1, Arg2, Arg3, Arg4, Arg5>(obj, m, a1, a2, a3, a4, a5); }
    template<class Obj, class Method, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6>
    Command* MC(Obj* obj, Method m, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6) { return new Internal::Command6<Obj, Method, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>(obj, m, a1, a2, a3, a4, a5, a6); }
    template<class Obj, class Method, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7>
    Command* MC(Obj* obj, Method m, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6, Arg7 a7) { return new Internal::Command7<Obj, Method, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>(obj, m, a1, a2, a3, a4, a5, a6, a7); }
    template<class Obj, class Method, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8>
    Command* MC(Obj* obj, Method m, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6, Arg7 a7, Arg8 a8) { return new Internal::Command8<Obj, Method, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8>(obj, m, a1, a2, a3, a4, a5, a6, a7, a8); }

    /*
    Releases object when executed.
    */
    class ReleaseCommand : public Command
    {
    public:
        ReleaseCommand(Object* object) : _object(object)
        { }

        virtual void Execute(ExecutionContext* cmdHandler) { if (_object) _object->Release(); }

    private:
        Object* _object;
    };
}