#pragma once

#include <map>
#include <utility>

namespace utility {
namespace maptranslation {

template<typename T1, typename T2>
const T2 & Translate(const std::map<T1, T2> & theMap, const T1 & key)
{
    auto it = theMap.find(key);
    if (it == theMap.end())
        throw std::runtime_error("Cannot find key in map");
    return it->second;
}

template<typename T1, typename T2>
const T2& Translate(const std::map<T1, T2> & theMap, const T1 & key, const T2 & defaultValue) noexcept
{
    auto it = theMap.find(key);
    if (it == theMap.end())
        return defaultValue;
    return it->second;
}

} // namespace maptranslation
} // namespace utility
