#include "utility/Serialization.h"

#include <bitset>
#include <iomanip>
#include <sstream>
#include "osal/StringConversion.h"
#include "utility/Assert.h"

namespace utility {

std::string Serialize(bool value, int width)
{
    return Align(std::string(value ? "true" : "false"), width);
}

std::string Serialize(int8_t value, int width, int base)
{
    std::ostringstream stream;

    switch (base)
    {
        case 2:
            {
                std::bitset<8> x(static_cast<uint8_t>(value));
                stream << std::setfill('0') << std::setw(8) << x;
            }
            break;
        case 8:
            {
                stream << std::oct << std::setfill('0') << std::setw(3) << (int)value;
            }
            break;
        case 10:
            {
                stream << std::dec << std::setfill('0') << (int)value;
            }
            break;
        case 16:
            {
                stream << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << (int)value;
            }
            break;
        default:
            ASSERT(false);
    }
    return Align(stream.str(), width);
}

std::string Serialize(uint8_t value, int width, int base)
{
    std::ostringstream stream;

    switch (base)
    {
        case 2:
            {
                std::bitset<8> x(value);
                stream << std::setfill('0') << std::setw(8) << x;
            }
            break;
        case 8:
            {
                stream << std::oct << std::setfill('0') << std::setw(3) << (int)value;
            }
            break;
        case 10:
            {
                stream << std::dec << std::setfill('0') << (int)value;
            }
            break;
        case 16:
            {
                stream << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << (int)value;
            }
            break;
        default:
            ASSERT(false);
    }
    return Align(stream.str(), width);
}

std::string Serialize(int16_t value, int width, int base)
{
    std::ostringstream stream;

    switch (base)
    {
        case 2:
            {
                std::bitset<16> x(static_cast<uint16_t>(value));
                stream << std::setfill('0') << std::setw(16) << x;
            }
            break;
        case 8:
            {
                stream << std::oct << std::setfill('0') << std::setw(6) << value;
            }
            break;
        case 10:
            {
                stream << std::dec << std::setfill('0') << value;
            }
            break;
        case 16:
            {
                stream << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << value;
            }
            break;
        default:
            ASSERT(false);
    }
    return Align(stream.str(), width);
}

std::string Serialize(uint16_t value, int width, int base)
{
    std::ostringstream stream;

    switch (base)
    {
        case 2:
            {
                std::bitset<16> x(value);
                stream << std::setfill('0') << std::setw(16) << x;
            }
            break;
        case 8:
            {
                stream << std::oct << std::setfill('0') << std::setw(6) << value;
            }
            break;
        case 10:
            {
                stream << std::dec << std::setfill('0') << value;
            }
            break;
        case 16:
            {
                stream << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << value;
            }
            break;
        default:
            ASSERT(false);
    }
    return Align(stream.str(), width);
}

std::string Serialize(int32_t value, int width, int base)
{
    std::ostringstream stream;

    switch (base)
    {
        case 2:
            {
                std::bitset<32> x(static_cast<uint32_t>(value));
                stream << std::setfill('0') << std::setw(32) << x;
            }
            break;
        case 8:
            {
                stream << std::oct << std::setfill('0') << std::setw(11) << value;
            }
            break;
        case 10:
            {
                stream << std::dec << std::setfill('0') << value;
            }
            break;
        case 16:
            {
                stream << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << value;
            }
            break;
        default:
            ASSERT(false);
    }
    return Align(stream.str(), width);
}

std::string Serialize(uint32_t value, int width, int base)
{
    std::ostringstream stream;

    switch (base)
    {
        case 2:
            {
                std::bitset<32> x(value);
                stream << std::setfill('0') << std::setw(32) << x;
            }
            break;
        case 8:
            {
                stream << std::oct << std::setfill('0') << std::setw(11) << value;
            }
            break;
        case 10:
            {
                stream << std::dec << std::setfill('0') << value;
            }
            break;
        case 16:
            {
                stream << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << value;
            }
            break;
        default:
            ASSERT(false);
    }
    return Align(stream.str(), width);
}

std::string Serialize(int64_t value, int width, int base)
{
    std::ostringstream stream;

    switch (base)
    {
        case 2:
            {
                std::bitset<64> x(static_cast<uint64_t>(value));
                stream << std::setfill('0') << std::setw(64) << x;
            }
            break;
        case 8:
            {
                stream << std::oct << std::setfill('0') << std::setw(22) << value;
            }
            break;
        case 10:
            {
                stream << std::dec << std::setfill('0') << value;
            }
            break;
        case 16:
            {
                stream << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << value;
            }
            break;
        default:
            ASSERT(false);
    }
    return Align(stream.str(), width);
}

std::string Serialize(uint64_t value, int width, int base)
{
    std::ostringstream stream;

    switch (base)
    {
        case 2:
            {
                std::bitset<64> x(value);
                stream << std::setfill('0') << std::setw(64) << x;
            }
            break;
        case 8:
            {
                stream << std::oct << std::setfill('0') << std::setw(22) << value;
            }
            break;
        case 10:
            {
                stream << std::dec << std::setfill('0') << value;
            }
            break;
        case 16:
            {
                stream << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << value;
            }
            break;
        default:
            ASSERT(false);
    }
    return Align(stream.str(), width);
}

std::string Serialize(float value, int width, int precision, FloatingPointRepresentation representation)
{
    std::ostringstream stream;

    switch (representation)
    {
        case FloatingPointRepresentation::Fixed:
            stream.setf(std::ios_base::floatfield, std::ios_base::fixed);
            stream << std::setfill('0') << std::setw(width) << std::setprecision(precision) << value;
            break;
        case FloatingPointRepresentation::Exponential:
            stream.setf(std::ios_base::floatfield, std::ios_base::scientific);
            stream << std::setfill('0') << std::setw(width) << std::setprecision(precision) << value;
            break;
        case FloatingPointRepresentation::Mixed:
            stream.unsetf(std::ios_base::floatfield);
            stream << std::setfill('0') << std::setprecision(precision) << value;
            break;
    }

    return Align(stream.str(), width);
}

std::string Serialize(double value, int width, int precision, FloatingPointRepresentation representation)
{
    std::ostringstream stream;

    switch (representation)
    {
        case FloatingPointRepresentation::Fixed:
            stream.setf(std::ios_base::floatfield, std::ios_base::fixed);
            stream << std::setfill('0') << std::setw(width) << std::setprecision(precision) << value;
            break;
        case FloatingPointRepresentation::Exponential:
            stream.setf(std::ios_base::floatfield, std::ios_base::scientific);
            stream << std::setfill('0') << std::setw(width) << std::setprecision(precision) << value;
            break;
        case FloatingPointRepresentation::Mixed:
            stream.unsetf(std::ios_base::floatfield);
            stream << std::setfill('0') << std::setprecision(precision) << value;
            break;
    }

    return Align(stream.str(), width);
}

std::string Serialize(const std::string & value, int width, bool quote)
{
    std::ostringstream stream;

    if (quote)
        stream << "\"";
    stream << value;
    if (quote)
        stream << "\"";

    return Align(stream.str(), width);
}

std::string Serialize(const std::wstring & value, int width, bool quote)
{
    std::ostringstream stream;

    stream << Serialize(osal::WStringToString(value), width, quote);

    return Align(stream.str(), width);
}

std::string Serialize(const char * value, int width, bool quote)
{
    std::ostringstream stream;

    stream << Serialize(std::string(value), width, quote);

    return Align(stream.str(), width);
}

std::string Serialize(const wchar_t * value, int width, bool quote)
{
    std::ostringstream stream;

    stream << Serialize(std::wstring(value), width, quote);

    return Align(stream.str(), width);
}

} // namespace utility
