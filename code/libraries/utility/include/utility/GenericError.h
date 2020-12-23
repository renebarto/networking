#pragma once

#include <ostream>
#include <sstream>
#include <string>
#include "utility/Format.h"

namespace utility {
    
class GenericError {
public:
    explicit GenericError(const std::string & text);
    template<typename ... Args>
    GenericError(const std::string & text, Args... args);

    GenericError &
    operator <<(
        const std::string & freeText);

    std::string Message() const { return m_text; }

private:
    std::string m_text;
};

std::ostream & operator <<(std::ostream & stream, const GenericError & value);

inline std::string Serialize(const GenericError & value)
{
    std::ostringstream stream;
    stream << value;
    return stream.str();
}

template<typename ... Args>
GenericError::GenericError(const std::string & text, Args... args)
    : m_text()
{
    utility::Format(m_text, text, args...);
}

} // namespace utility