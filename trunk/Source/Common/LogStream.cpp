#include "Includes.h"
#include "LogStream.h"
#include "Logger.h"

namespace P3D
{
    LogStream::~LogStream()
    {
        if (!_buffer.empty()) 
        {
            if (_buffer[_buffer.size() - 1] != '.' && _buffer[_buffer.size() - 1] != '!') _buffer += '.';
            _owner->Message(_level, _buffer.c_str());
        }
    }
}