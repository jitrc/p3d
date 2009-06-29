#include "Includes.h"
#include "Exceptions.h"

namespace P3D
{
    Exception::Exception(const String& message, const Exception* inner)
        : _message(message), _inner(inner!=NULL ? inner->GetFullDescription() : L"")
    { 
    }

    const String Exception::GetFullDescription() const
    {
        std::wostringstream os;
        os << L"Exception (0x" << std::hex << this << L") { ";
        if (!_message.empty()) os << L"'" << _message << L"'";
        if (!_inner.empty()) 
            os << L", inner is " << _inner << L" }";
        else
            os << L" }";
        return os.str();
    }
}