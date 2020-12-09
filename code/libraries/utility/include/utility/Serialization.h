#pragma once

#include <string>
#include "utility/StringFunctions.h"

namespace utility {

// String serialization
// Every serialization specialization uses a width parameter, which is used for expansion
// width < 0 Left aligned
// width > 0 right aligned
// width < actual length no alignment
std::string Serialize(bool value, int width = 0);
std::string Serialize(int8_t value, int width = 0, int base = 10);
std::string Serialize(uint8_t value, int width = 0, int base = 10);
std::string Serialize(int16_t value, int width = 0, int base = 10);
std::string Serialize(uint16_t value, int width = 0, int base = 10);
std::string Serialize(int32_t value, int width = 0, int base = 10);
std::string Serialize(uint32_t value, int width = 0, int base = 10);
std::string Serialize(int64_t value, int width = 0, int base = 10);
std::string Serialize(uint64_t value, int width = 0, int base = 10);

enum class FloatingPointRepresentation
{
    Fixed,
    Exponential,
    Mixed
};
std::string Serialize(float value, int width = 0, int precision = 16, FloatingPointRepresentation representation = FloatingPointRepresentation::Mixed);
std::string Serialize(double value, int width = 0, int precision = 16, FloatingPointRepresentation representation = FloatingPointRepresentation::Mixed);
std::string Serialize(long double value, int width = 0, int precision = 16, FloatingPointRepresentation representation = FloatingPointRepresentation::Mixed);
std::string Serialize(const std::string & value, int width = 0, bool quote = false);
std::string Serialize(const std::wstring & value, int width = 0, bool quote = false);
std::string Serialize(const char * value, int width = 0, bool quote = false);
std::string Serialize(const wchar_t * value, int width = 0, bool quote = false);

template<typename T>
std::string Serialize(const T & t, int width = 0)
{
    std::ostringstream stream;
    stream << t;
    return Align(stream.str(), width);
}

} // namespace utility

