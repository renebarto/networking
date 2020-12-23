#pragma once

#include <ostream>
#include <string>
#include <sstream>
#include "utility/Format.h"

namespace utility {
    
class Error {
public:
    Error(int errorCode, const std::string errorString, const std::string & text);
    template<typename ... Args>
    Error(int errorCode, const std::string errorString, const std::string & text, Args... args);

    Error &
    operator <<(
        const std::string & freeText);

    int ErrorCode() const { return m_errorCode; }
    std::string ErrorString() const { return m_errorString; }
    std::string Message() const { return m_text; }

private:
    int m_errorCode;
    std::string m_errorString;
    std::string m_text;
};

std::ostream & operator <<(std::ostream & stream, const Error & value);

inline std::string Serialize(const Error & value)
{
    std::ostringstream stream;
    stream << value;
    return stream.str();
}

template<typename ... Args>
Error::Error(int errorCode, const std::string errorString, const std::string & text, Args... args)
    : m_errorCode(errorCode)
    , m_errorString(errorString)
    , m_text()
{
    utility::Format(m_text, text, args...);
}

} // namespace utility