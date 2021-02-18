#pragma once

#include <chrono>
#include <sstream>
#include <string>
#include "utility/Endian.h"
#include "utility/StringFunctions.h"

namespace serialization {

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
std::string Serialize(const void * value, int width = 0);
std::string Serialize(void * value, int width = 0);
std::string Serialize(std::chrono::milliseconds & value, int width = 0);
std::string Serialize(std::chrono::seconds & value, int width = 0);
std::string SerializeData(const std::uint8_t * value, std::size_t size);
std::string SerializeData(const std::vector<std::uint8_t> & value);

template<typename T>
std::string Serialize(T t, int width)
{
    std::ostringstream stream;
    stream << t;
    return utility::Align(stream.str(), width);
}

// Binary Serialization

void AppendOrReplace(std::vector<std::uint8_t> & buffer, std::size_t & offset, const void * data, std::size_t length);

template <typename T>
typename std::enable_if<!std::is_enum<T>::value, void>::type
SerializeBinary(T value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness /*endianness = utility::Endianness::LittleEndian*/)
{
    AppendOrReplace(buffer, offset, &value, sizeof(value));
}

void SerializeBinary(bool value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
void SerializeBinary(std::int8_t value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
void SerializeBinary(std::uint8_t value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
void SerializeBinary(std::int16_t value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
void SerializeBinary(std::uint16_t value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
void SerializeBinary(std::int32_t value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
void SerializeBinary(std::uint32_t value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
void SerializeBinary(std::int64_t value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
void SerializeBinary(std::uint64_t value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
void SerializeBinary(float value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
void SerializeBinary(double value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
void SerializeBinary(long double value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
void SerializeBinary(const std::string & value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
void SerializeBinary(const std::wstring & value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
void SerializeBinary(const char * value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
void SerializeBinary(const wchar_t * value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
template<typename EnumType>
typename std::enable_if<std::is_enum<EnumType>::value, void>::type
SerializeBinary(const EnumType & value, std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian)
{
    SerializeBinary(static_cast<typename std::underlying_type<EnumType>::type>(value), buffer, offset, endianness);
}


template<class T>
class IStringSerializer
{
public:
    IStringSerializer()
    {}

    virtual ~IStringSerializer()
    {}

    virtual void Serialize(std::string & text, const T & result) = 0;
};

} // namespace serialization
