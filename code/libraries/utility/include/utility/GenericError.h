#pragma once

#include <ostream>
#include <string>

namespace utility {
    
class GenericError {
public:
    explicit GenericError(const std::string & text);

    GenericError &
    operator <<(
        const std::string & freeText);

    std::string Message() const { return m_text; }

private:
    std::string m_text;
};

std::ostream & operator <<(std::ostream & stream, const GenericError & value);

} // namespace utility