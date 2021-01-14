#include "utility/Serialization.h"

#include <algorithm>
#include <bitset>
#include <cstring>
#include <iomanip>
#include <sstream>
#include "osal/StringConversion.h"
#include "utility/Assert.h"
#include "utility/TypeCast.h"

namespace serialization {

std::string Serialize(bool value, int width)
{
    return utility::Align(std::string(value ? "true" : "false"), width);
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
                stream << std::oct << std::setfill('0') << std::setw(3) << static_cast<int>(value) << std::dec;
            }
            break;
        case 10:
            {
                stream << std::dec << std::setfill('0') << static_cast<int>(value);
            }
            break;
        case 16:
            {
                stream << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<int>(value) << std::dec;
            }
            break;
        default:
            ASSERT(false);
    }
    return utility::Align(stream.str(), width);
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
                stream << std::oct << std::setfill('0') << std::setw(3) << static_cast<int>(value) << std::dec;
            }
            break;
        case 10:
            {
                stream << std::dec << std::setfill('0') << static_cast<int>(value);
            }
            break;
        case 16:
            {
                stream << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<int>(value) << std::dec;
            }
            break;
        default:
            ASSERT(false);
    }
    return utility::Align(stream.str(), width);
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
                stream << std::oct << std::setfill('0') << std::setw(6) << value << std::dec;
            }
            break;
        case 10:
            {
                stream << std::dec << std::setfill('0') << value;
            }
            break;
        case 16:
            {
                stream << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << value << std::dec;
            }
            break;
        default:
            ASSERT(false);
    }
    return utility::Align(stream.str(), width);
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
                stream << std::oct << std::setfill('0') << std::setw(6) << value << std::dec;
            }
            break;
        case 10:
            {
                stream << std::dec << std::setfill('0') << value;
            }
            break;
        case 16:
            {
                stream << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << value << std::dec;
            }
            break;
        default:
            ASSERT(false);
    }
    return utility::Align(stream.str(), width);
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
                stream << std::oct << std::setfill('0') << std::setw(11) << value << std::dec;
            }
            break;
        case 10:
            {
                stream << std::dec << std::setfill('0') << value;
            }
            break;
        case 16:
            {
                stream << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << value << std::dec;
            }
            break;
        default:
            ASSERT(false);
    }
    return utility::Align(stream.str(), width);
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
                stream << std::oct << std::setfill('0') << std::setw(11) << value << std::dec;
            }
            break;
        case 10:
            {
                stream << std::dec << std::setfill('0') << value;
            }
            break;
        case 16:
            {
                stream << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << value << std::dec;
            }
            break;
        default:
            ASSERT(false);
    }
    return utility::Align(stream.str(), width);
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
                stream << std::oct << std::setfill('0') << std::setw(22) << value << std::dec;
            }
            break;
        case 10:
            {
                stream << std::dec << std::setfill('0') << value;
            }
            break;
        case 16:
            {
                stream << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << value << std::dec;
            }
            break;
        default:
            ASSERT(false);
    }
    return utility::Align(stream.str(), width);
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
                stream << std::oct << std::setfill('0') << std::setw(22) << value << std::dec;
            }
            break;
        case 10:
            {
                stream << std::dec << std::setfill('0') << value;
            }
            break;
        case 16:
            {
                stream << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << value << std::dec;
            }
            break;
        default:
            ASSERT(false);
    }
    return utility::Align(stream.str(), width);
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

    return utility::Align(stream.str(), width);
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

    return utility::Align(stream.str(), width);
}

std::string Serialize(const std::string & value, int width, bool quote)
{
    std::ostringstream stream;

    if (quote)
        stream << "\"";
    stream << value;
    if (quote)
        stream << "\"";

    return utility::Align(stream.str(), width);
}

std::string Serialize(const std::wstring & value, int width, bool quote)
{
    std::ostringstream stream;

    stream << Serialize(osal::WStringToString(value), width, quote);

    return utility::Align(stream.str(), width);
}

std::string Serialize(const char * value, int width, bool quote)
{
    std::ostringstream stream;

    stream << Serialize(std::string(value), width, quote);

    return utility::Align(stream.str(), width);
}

std::string Serialize(const wchar_t * value, int width, bool quote)
{
    std::ostringstream stream;

    stream << Serialize(std::wstring(value), width, quote);

    return utility::Align(stream.str(), width);
}

std::string Serialize(const void * value, int width)
{
    std::ostringstream stream;

    if (value != nullptr)
    {
        stream << "0x" + Serialize(CastPointerToInteger(value), width, 16);
    }
    else
        stream << "null";

    return utility::Align(stream.str(), width);
}

std::string Serialize(void * value, int width)
{
    return Serialize(const_cast<const void *>(value), width);
}

std::string SerializeData(const std::uint8_t * value, std::size_t size)
{
    std::ostringstream stream;

    if (value != nullptr)
    {
        const std::size_t LineDisplayBytes = 16;
        const std::size_t MaxDisplayBytes = 32;
        std::size_t displayBytes = (size < MaxDisplayBytes) ? size : MaxDisplayBytes;
        for (std::size_t i = 0; i < displayBytes; i += LineDisplayBytes)
        {
            for (std::size_t j = 0; j < LineDisplayBytes; ++j)
            {
                if (j != 0)
                    stream << " ";
                if ((i + j) < displayBytes)
                    stream << Serialize(value[i + j], 2, 16);
                else
                    stream << "  ";
            }
            stream << "  ";
            for (std::size_t j = 0; j < LineDisplayBytes; ++j)
            {
                if (j != 0)
                    stream << " ";
                if ((i + j) < displayBytes)
                {
                    auto ch = value[i + j];
                    stream << ((std::isprint(ch) ? static_cast<char>(ch) : '.'));
                }
                else
                {
                    stream << " ";
                }
            }
            stream << std::endl;
        }
    }
    else
        stream << "null";
    return stream.str();
}

// Binary Serialization

void AppendOrReplace(std::vector<std::uint8_t> & buffer, std::size_t & offset, const void * data, std::size_t length)
{
    if (buffer.size() < offset + length)
        buffer.resize(offset + length);
    const std::uint8_t * bytePtr = reinterpret_cast<const std::uint8_t *>(data);
    std::copy(bytePtr, bytePtr + length, &buffer[offset]);
    offset += length;
}

void SerializeBinary(bool value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness /*endianness*/)
{
    std::uint8_t data = static_cast<std::uint8_t>(value ? 0x01 : 0x00);
    AppendOrReplace(buffer, offset, &data, sizeof(data));
}

void SerializeBinary(std::int8_t value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness /*endianness*/)
{
    std::uint8_t data = static_cast<std::uint8_t>(value);
    AppendOrReplace(buffer, offset, &data, sizeof(data));
}

void SerializeBinary(std::uint8_t value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness /*endianness*/)
{
    AppendOrReplace(buffer, offset, &value, sizeof(value));
}

void SerializeBinary(std::int16_t value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness)
{
    std::int16_t convertedValue = utility::ToEndianness(value, endianness);
    const std::uint8_t * data = reinterpret_cast<const std::uint8_t *>(&convertedValue);
    AppendOrReplace(buffer, offset, data, sizeof(convertedValue));
}

void SerializeBinary(std::uint16_t value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness)
{
    std::uint16_t convertedValue = utility::ToEndianness(value, endianness);
    const std::uint8_t * data = reinterpret_cast<const std::uint8_t *>(&convertedValue);
    AppendOrReplace(buffer, offset, data, sizeof(convertedValue));
}

void SerializeBinary(std::int32_t value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness)
{
    std::int32_t convertedValue = utility::ToEndianness(value, endianness);
    const std::uint8_t * data = reinterpret_cast<const std::uint8_t *>(&convertedValue);
    AppendOrReplace(buffer, offset, data, sizeof(convertedValue));
}

void SerializeBinary(std::uint32_t value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness)
{
    std::uint32_t convertedValue = utility::ToEndianness(value, endianness);
    const std::uint8_t * data = reinterpret_cast<const std::uint8_t *>(&convertedValue);
    AppendOrReplace(buffer, offset, data, sizeof(convertedValue));
}

void SerializeBinary(std::int64_t value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness)
{
    std::int64_t convertedValue = utility::ToEndianness(value, endianness);
    const std::uint8_t * data = reinterpret_cast<const std::uint8_t *>(&convertedValue);
    AppendOrReplace(buffer, offset, data, sizeof(convertedValue));
}

void SerializeBinary(std::uint64_t value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness)
{
    std::uint64_t convertedValue = utility::ToEndianness(value, endianness);
    const std::uint8_t * data = reinterpret_cast<const std::uint8_t *>(&convertedValue);
    AppendOrReplace(buffer, offset, data, sizeof(convertedValue));
}

void SerializeBinary(float value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness)
{
    const std::uint32_t convertedValue = utility::ToEndianness(*reinterpret_cast<const std::uint32_t *>(&value), endianness);
    const std::uint8_t * data = reinterpret_cast<const std::uint8_t *>(&convertedValue);
    AppendOrReplace(buffer, offset, data, sizeof(convertedValue));
}

void SerializeBinary(double value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness)
{
    const std::uint64_t convertedValue = utility::ToEndianness(*reinterpret_cast<const std::uint64_t *>(&value), endianness);
    const std::uint8_t * data = reinterpret_cast<const std::uint8_t *>(&convertedValue);
    AppendOrReplace(buffer, offset, data, sizeof(convertedValue));
}

void SerializeBinary(long double value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness)
{
    std::uint8_t convertedValue[16] {};
    std::copy(reinterpret_cast<const std::uint8_t *>(&value), reinterpret_cast<const std::uint8_t *>(&value) + sizeof(value), std::begin(convertedValue));
    if (endianness != utility::PlatformEndianness())
    {
        std::reverse(std::begin(convertedValue), std::end(convertedValue));
    }
    const std::uint8_t * data = convertedValue;
    AppendOrReplace(buffer, offset, data, sizeof(convertedValue));
}

void SerializeBinary(const std::string & value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness)
{
    SerializeBinary(value.length(), buffer, offset, endianness);
    for (auto ch : value)
    {
        SerializeBinary(CastToInteger(ch), buffer, offset, endianness);
    }
}

void SerializeBinary(const std::wstring & value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness)
{
    SerializeBinary(value.length(), buffer, offset, endianness);
    for (auto ch : value)
    {
        SerializeBinary(CastToInteger(ch), buffer, offset, endianness);
    }
}

void SerializeBinary(const char * value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness)
{
    auto length = std::strlen(value);
    SerializeBinary(length, buffer, offset, endianness);
    for (std::size_t i = 0; i < length; ++i)
    {
        SerializeBinary(CastToInteger(value[i]), buffer, offset, endianness);
    }
}

void SerializeBinary(const wchar_t * value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness)
{
    auto length = std::wcslen(value);
    SerializeBinary(length, buffer, offset, endianness);
    for (std::size_t i = 0; i < length; ++i)
    {
        SerializeBinary(CastToInteger(value[i]), buffer, offset, endianness);
    }
}

} // namespace serialization
