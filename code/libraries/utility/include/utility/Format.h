#pragma once

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>
#include "utility/Deserialization.h"
#include "utility/Serialization.h"

namespace utility {
    
// Printing with C# like formatting
// Format:
// {<index><,width<:format>>}
// If index is left out, the logical ordering applies (0, 1, 2, 3...)
// If index is specified, it refers to the parameter with specified index, counting from 0
// If width is left out, the width needed to print is used
// If width is specified, this width will be used (right aligned for positive with left aligned for negative width), unless the width needed is larger
// If format is left out, the standard printing format is used
// If format is specified, the type of argument defines how the format is used:
// For integral values:
//      D<n> for decimal values with n digits (leading zeros if needed, actual width if n is too small)
//      O<n> for octal values with n digits (leading zeros if needed, actual width if n is too small)
//      B<n> for binary values with n digits (leading zeros if needed, actual width if n is too small)
//      X<n> for hexadecimal values with n digits (leading zeros if needed, actual width if n is too small)
// For floating point values:
//      F<n> for fixed representation using n fractional digits
//      E<n> for exponential representation using n fractional digits
//      G<n> for fixed or exponential representation (whichever fits best) (n is ignored)

const std::string & GetFormatError();
bool HaveFormatError();
void ResetFormatError();
void SetFormatError(const std::string & message);

template<typename T, typename V>
typename std::enable_if<!std::is_integral<V>::value && !std::is_floating_point<V>::value, std::basic_string<T>>::type
Format(V value, int width, const std::basic_string<T> & /*format*/)
{
    ResetFormatError();
    return Serialize(value, width);
};

template<typename T, typename V>
typename std::enable_if<std::is_integral<V>::value, std::basic_string<T>>::type
Format(V value, int width, const std::basic_string<T> & format)
{
    ResetFormatError();
    if (!format.empty())
    {
        switch (format[0])
        {
            case 'D':
                {
                    int digits {};
                    if (format.length() > 1)
                        Deserialize(format.substr(1), digits);
                    return Align(Serialize(value, digits, 10), width);
                }
                break;
            case 'O':
                {
                    int digits {};
                    if (format.length() > 1)
                        Deserialize(format.substr(1), digits);
                    return Align(Serialize(value, digits, 8), width);
                }
                break;
            case 'B':
                {
                    int digits {};
                    if (format.length() > 1)
                        Deserialize(format.substr(1), digits);
                    return Align(Serialize(value, digits, 2), width);
                }
                break;
            case 'X':
                {
                    int digits {};
                    if (format.length() > 1)
                        Deserialize(format.substr(1), digits);
                    return Align(Serialize(value, digits, 16), width);
                }
                break;
            default:
                return Serialize(value, width);
        }
    }
    return Serialize(value, width);
};

template<typename T, typename V>
typename std::enable_if<std::is_floating_point<V>::value, std::basic_string<T>>::type
Format(V value, int width, const std::basic_string<T> & format)
{
    ResetFormatError();
    if (!format.empty())
    {
        switch (format[0]) {
            case 'F':
                {
                    std::basic_string<T> subFormat = format.substr(1);
                    auto decimalPointIndex = subFormat.find(T('.'));
                    std::basic_string<T> integralDecimalsSpecifier = subFormat;
                    std::basic_string<T> fractionalDecimalsSpecifier;
                    if (decimalPointIndex != std::basic_string<T>::npos)
                    {
                        integralDecimalsSpecifier = subFormat.substr(0, decimalPointIndex);
                        fractionalDecimalsSpecifier = subFormat.substr(decimalPointIndex + 1);
                    }
                    int integralDecimals {};
                    int fractionalDecimals {};
                    Deserialize(integralDecimalsSpecifier, integralDecimals);
                    Deserialize(fractionalDecimalsSpecifier, fractionalDecimals);
                    return Align(Serialize(value, integralDecimals + fractionalDecimals + 1, fractionalDecimals,
                                           FloatingPointRepresentation::Fixed), width);
                }
                break;
            case 'E':
                {
                    std::basic_string<T> subFormat = format.substr(1);
                    auto decimalPointIndex = subFormat.find(T('.'));
                    std::basic_string<T> integralDecimalsSpecifier = subFormat;
                    std::basic_string<T> fractionalDecimalsSpecifier;
                    if (decimalPointIndex != std::basic_string<T>::npos)
                    {
                        integralDecimalsSpecifier = subFormat.substr(0, decimalPointIndex);
                        fractionalDecimalsSpecifier = subFormat.substr(decimalPointIndex + 1);
                    }
                    int integralDecimals {};
                    int fractionalDecimals {};
                    Deserialize(integralDecimalsSpecifier, integralDecimals);
                    Deserialize(fractionalDecimalsSpecifier, fractionalDecimals);
                    return Align(Serialize(value, integralDecimals + fractionalDecimals + 1, fractionalDecimals,
                                           FloatingPointRepresentation::Exponential), width);
                }
                break;
            case 'G':
                {
                    std::basic_string<T> subFormat = format.substr(1);
                    auto decimalPointIndex = subFormat.find(T('.'));
                    std::basic_string<T> integralDecimalsSpecifier = subFormat;
                    std::basic_string<T> fractionalDecimalsSpecifier;
                    if (decimalPointIndex != std::basic_string<T>::npos)
                    {
                        integralDecimalsSpecifier = subFormat.substr(0, decimalPointIndex);
                        fractionalDecimalsSpecifier = subFormat.substr(decimalPointIndex + 1);
                    }
                    int integralDecimals {};
                    int fractionalDecimals {};
                    Deserialize(integralDecimalsSpecifier, integralDecimals);
                    Deserialize(fractionalDecimalsSpecifier, fractionalDecimals);
                    // We need different behaviour here due to how default output of floating point numbers works
                    return Align(Serialize(value, 0, integralDecimals + fractionalDecimals + 1,
                                           FloatingPointRepresentation::Mixed), width);
                }
                break;
            default:
                return Serialize(value, width);
        }
    }
    return Serialize(value, width);
};

template<typename T>
void Format(std::basic_string<T> & /*buffer*/,
            size_t /*index*/, size_t /*parameterIndex*/,
            int /*parameterWidth*/,
            const std::basic_string<T> & /*parameterFormat*/)
{
    ResetFormatError();
}

template<typename T, typename V, typename ... Args>
void Format(std::basic_string<T> & buffer,
            size_t index, size_t parameterIndex,
            int parameterWidth,
            const std::basic_string<T> & parameterFormat,
            V value, Args ... FArgs)
{
    ResetFormatError();
    if (index == parameterIndex)
    {
        buffer += Format<T>(value, parameterWidth, parameterFormat);
    }
    else
        Format(buffer, index, parameterIndex + 1, parameterWidth, parameterFormat, FArgs ...);
}

template <typename T, typename ... Args>
void Format(std::basic_string<T> & buffer,
            const T * format,
            Args const & ... args)
{
    ResetFormatError();
    size_t numParameters = sizeof...(args);
    std::vector<bool> parameterUsed(numParameters);
    const T * currentPosition = format;
    size_t parameterOrdinal {};
    while (*currentPosition != T('\0'))
    {
        const T * parameterStartIndication = std::strchr(currentPosition, T('{'));
        if (parameterStartIndication != nullptr)
        {
            buffer += std::basic_string<T>(currentPosition, static_cast<size_t>(parameterStartIndication - currentPosition));
            const T * parameterEndIndication = std::strchr(parameterStartIndication + 1, T('}'));
            if (parameterEndIndication != nullptr)
            {
                std::basic_string<T> parameterID;
                std::basic_string<T> widthSpecifier;
                std::basic_string<T> formatSpecifier;
                parameterID = std::basic_string<T>(parameterStartIndication + 1, static_cast<size_t>(parameterEndIndication - parameterStartIndication - 1));
                const T * parameterWidthDelimiter = std::strchr(parameterStartIndication + 1, T(','));
                if ((parameterWidthDelimiter != nullptr) && (parameterWidthDelimiter < parameterEndIndication))
                {
                    parameterID = std::basic_string<T>(parameterStartIndication + 1, static_cast<size_t>(parameterWidthDelimiter - parameterStartIndication - 1));
                    widthSpecifier = std::basic_string<T>(parameterWidthDelimiter + 1, static_cast<size_t>(parameterEndIndication - parameterWidthDelimiter - 1));
                    const T * parameterFormatDelimiter = std::strchr(parameterWidthDelimiter + 1, T(':'));
                    if ((parameterFormatDelimiter != nullptr) && (parameterFormatDelimiter < parameterEndIndication))
                    {
                        widthSpecifier = std::basic_string<T>(parameterWidthDelimiter + 1, static_cast<size_t>(parameterFormatDelimiter - parameterWidthDelimiter - 1));
                        formatSpecifier = std::basic_string<T>(parameterFormatDelimiter + 1, static_cast<size_t>(parameterEndIndication - parameterFormatDelimiter - 1));
                    }
                }
                size_t parameterIndex = parameterOrdinal;
                if (!parameterID.empty() && !Deserialize(parameterID, parameterIndex))
                {
                    std::ostringstream stream;
                    stream << "Invalid index specified: " << parameterID;
                    SetFormatError(stream.str());
                    return;
                }
                if (parameterIndex < 0 || parameterIndex >= numParameters)
                {
                    std::ostringstream stream;
                    stream << "Invalid index specified: " << parameterID << " should be between 0 and " << numParameters - 1;
                    SetFormatError(stream.str());
                    return;
                }
                int width {};
                if (!widthSpecifier.empty() && !Deserialize(widthSpecifier, width))
                {
                    std::ostringstream stream;
                    stream << "Invalid width specified: " << widthSpecifier;
                    SetFormatError(stream.str());
                    return;
                }
                parameterUsed[parameterIndex] = true;
                Format(buffer, parameterIndex, 0, width, formatSpecifier, args ...);

                currentPosition = parameterEndIndication + 1;
            } else
            {
                break;
            }
            ++parameterOrdinal;
        } 
        else
        {
            buffer += std::basic_string<T>(currentPosition);
            break;
        }
    }
    bool haveUnusedElements = false;
    std::ostringstream stream;
    for (size_t i = 0; i < numParameters; ++i)
    {
        if (!parameterUsed[i])
        {
            if (!haveUnusedElements)
            {
                stream << "Not all parameters have been used. Parameters not used:";
                haveUnusedElements = true;
            }
            stream << " " << i;
        }
    }
    if (haveUnusedElements)
    {
        SetFormatError(stream.str());
    }
}

template <typename T, typename ... Args>
void Format(std::basic_string<T> & buffer,
            const std::basic_string<T> & format,
            Args const & ... args)
{
    Format(buffer, format.c_str(), args ...);
}

inline void Format(std::string & buffer, const char * value) noexcept
{
    Format(buffer, "{}", value);
}

inline void Format(std::string & buffer, const wchar_t * value) noexcept
{
    Format(buffer, "{}", value);
}

inline void Format(std::string & buffer, std::wstring const & value) noexcept
{
    Format(buffer, "{}", value);
}

template <typename T, typename ... Args>
std::basic_string<T> FormatString(
            const std::basic_string<T> & format,
            Args const & ... args)
{
    std::basic_string<T> buffer;
    Format(buffer, format.c_str(), args ...);
    return buffer;
}

} // namespace utility