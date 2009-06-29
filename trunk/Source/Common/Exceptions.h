#pragma once

namespace P3D
{
    /*
    Base class for all exceptions.
    */
    class Exception : public std::exception
    {
    public:
        Exception(const String& message, const Exception* inner = NULL);

        const String& GetInnerExceptionMessage() const { return _inner; };
        const String& GetExceptionMessage() const { return _message; }

        /*
        Returns string with description of this exception and all inner exceptions.
        */
        virtual const String GetFullDescription() const;

    protected:
        const String _inner;
        const String _message;
    };

    /*
    Exception that terminates the program.
    */
    class FatalException : public Exception
    {
    public:
        FatalException(const String& message, const Exception* inner = NULL)
            : Exception(message, inner)
        { }
    };

    /*
    The operation is not supported.
    */
    class NotSupportedException : public Exception
    {
    public:
        NotSupportedException()
            : Exception(L"The operation is not supported.", NULL)
        { }

        NotSupportedException(const String& message, const Exception* inner = NULL)
            : Exception(message, inner)
        { }
    };
}