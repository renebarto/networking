#include "network/IPV6Address.h"

#include <iomanip>

namespace network {

IPV6Address const IPV6Address::None = IPV6Address({0, 0, 0, 0, 0, 0, 0, 0,
                                                   0, 0, 0, 0, 0, 0, 0, 0});
IPV6Address const IPV6Address::Any = IPV6Address({0, 0, 0, 0, 0, 0, 0, 0,
                                                  0, 0, 0, 0, 0, 0, 0, 0});
IPV6Address const IPV6Address::Broadcast = IPV6Address({255, 255, 255, 255, 255, 255, 255, 255,
                                                        255, 255, 255, 255, 255, 255, 255, 255});
IPV6Address const IPV6Address::LocalHost = IPV6Address({0, 0, 0, 0, 0, 0, 0, 0,
                                                        0, 0, 0, 0, 0, 0, 0, 1});

std::ostream & operator << (std::ostream & stream, const IPV6Address & value)
{
    constexpr size_t NumWords = IPV6Address::AddressSize / 2;
    std::uint16_t words[NumWords];
    size_t zeroSequenceStartMax = 0;
    size_t zeroSequenceLengthMax = 0;
    size_t zeroSequenceStart = 0;
    size_t zeroSequenceLength = 0;
    bool inZeroSequence = false;
    size_t offset = 0;
    for (size_t i = 0; i < NumWords; ++i)
    {
        words[i] = static_cast<std::uint16_t>(value.m_address[offset] << 8 | value.m_address[offset + 1]);
        offset += 2;
        if (words[i] == 0)
        {
            if (!inZeroSequence)
            {
                zeroSequenceStart = i;
                inZeroSequence = true;
            }
        }
        else
        {
            if (inZeroSequence)
            {
                zeroSequenceLength = i - zeroSequenceStart;
                inZeroSequence = false;
                if (zeroSequenceLength > zeroSequenceLengthMax)
                {
                    zeroSequenceStartMax = zeroSequenceStart;
                    zeroSequenceLengthMax = zeroSequenceLength;
                }
                zeroSequenceStart = static_cast<size_t>(-1);
                zeroSequenceLength = 0;
            }
        }
    }
    if (inZeroSequence)
    {
        zeroSequenceLength = NumWords - zeroSequenceStart;
        if (zeroSequenceLength > zeroSequenceLengthMax)
        {
            zeroSequenceStartMax = zeroSequenceStart;
            zeroSequenceLengthMax = zeroSequenceLength;
        }

    }
    stream << std::hex;
    for (size_t i = 0; i < NumWords; ++i)
    {
        if ((i == zeroSequenceStartMax) && (zeroSequenceLengthMax > 1))
        {
            stream << "::";
        }
        else if ((i == zeroSequenceStartMax) && (zeroSequenceLengthMax == 1))
        {
            stream << ":0:";
        }
        else if ((i < zeroSequenceStartMax) || (i >= zeroSequenceStartMax + zeroSequenceLengthMax))
        {
            stream << words[i];
            if (((i + 1) < NumWords) && ((i + 1) != zeroSequenceStartMax))
                stream << ":";
        }
    }
    return stream << std::dec;
}

bool operator == (const IPV6Address & lhs, const IPV6Address & rhs)
{
    return (lhs.m_address == rhs.m_address);
}
bool operator == (const IPV6Address::AddressType & lhs, const IPV6Address & rhs)
{
    return (lhs == rhs.m_address);
}
bool operator == (const IPV6Address & lhs, const IPV6Address::AddressType & rhs)
{
    return (lhs.m_address == rhs);
}
bool operator != (const IPV6Address & lhs, const IPV6Address & rhs)
{
    return !operator == (lhs, rhs);
}
bool operator != (const IPV6Address::AddressType & lhs, const IPV6Address & rhs)
{
    return !operator == (lhs, rhs);
}
bool operator != (const IPV6Address & lhs, const IPV6Address::AddressType & rhs)
{
    return !operator == (lhs, rhs);
}

} // namespace network