#pragma once

#include <ostream>
#include <string>
#include <sstream>

namespace utility {
    
class Error {
public:
    explicit Error(const std::string & text);
    Error(int errorCode, const std::string errorString, const std::string & text);

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

} // namespace utility