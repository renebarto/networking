#include "utility/Endian.h"

namespace serialization {

template<>
const utility::BidirectionalMap<utility::Endianness, std::string> EnumSerializationMap<utility::Endianness>::ConversionMap = 
{
    {utility::Endianness::LittleEndian, "LittleEndian"},
    {utility::Endianness::BigEndian, "BigEndian"},
};

} // namespace serialization
