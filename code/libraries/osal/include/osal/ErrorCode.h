//---------------------------------------------------------------
// Copyright   : Copyright(c) 2008, Koninklijke Philips Electronics N.V.
//
// File        : ErrorCode.h
//
// Namespace   : osal
//
// Class       : ErrorCode
//
//
// Description :
//     Event definitions
//
//---------------------------------------------------------------

#pragma once

#include <algorithm>
#include <iostream>
#include <string>

namespace osal {

class ErrorCode
{
public:
    ErrorCode()
        : m_errorCode(0)
    {}
    explicit ErrorCode(int errorCode)
        : m_errorCode(errorCode)
    {}
    ErrorCode(const ErrorCode & other)
        : m_errorCode(other.m_errorCode)
    {
    }
    ErrorCode& operator = (const ErrorCode & other)
    {
        if (this != & other)
        {
            m_errorCode = other.m_errorCode;
        }
        return *this;
    }

    int Code() const { return m_errorCode; }
    std::string Description() const;
    operator bool() const { return m_errorCode == 0; }

private:
    int m_errorCode;
};

inline std::ostream & operator << (std::ostream & stream, const ErrorCode & value)
{
    stream << value.Code() << ":" << value.Description();
    return stream;
}

} // namespace osal
