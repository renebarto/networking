//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020 Koninklijke Philips Electronics N.V.
//
// File        : TypeCast.h
//
// Namespace   : -
//
// Class       : -
//
// Description :
//
//------------------------------------------------------------------------------

#pragma once

template <typename P>
typename std::enable_if<sizeof(P) == 1, std::uint8_t>::type CastToInteger(P value)
{
    return static_cast<std::uint8_t>(value);
}
template <typename P>
typename std::enable_if<sizeof(P) == 2, std::uint16_t>::type CastToInteger(P value)
{
    return static_cast<std::uint16_t>(value);
}
template <typename P>
typename std::enable_if<sizeof(P) == 4, std::uint32_t>::type CastToInteger(P value)
{
    return static_cast<std::uint32_t>(value);
}
template <typename P>
typename std::enable_if<sizeof(P) == 8, std::uint64_t>::type CastToInteger(P value)
{
    return static_cast<std::uint64_t>(value);
}
template <typename P>
typename std::enable_if<sizeof(P) == 4, std::uint32_t>::type CastPointerToInteger(P value)
{
    return reinterpret_cast<std::uint32_t>(value);
}
template <typename P>
typename std::enable_if<sizeof(P) == 8, std::uint64_t>::type CastPointerToInteger(P value)
{
    return reinterpret_cast<std::uint64_t>(value);
}
