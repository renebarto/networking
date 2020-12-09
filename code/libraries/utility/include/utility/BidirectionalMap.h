#pragma once

#include <map>
#include <vector>
#include <utility>
#include <iterator>
#include "utility/MapUtilities.h"

namespace utility {

template<class T1, class T2>
class BidirectionalMap
{
public:
    BidirectionalMap(std::initializer_list<std::pair<T1, T2>> values)
    {
        for (auto pair : values)
        {
            m_T1toT2Map.emplace(pair.first, pair.second);
            m_T2toT1Map.emplace(pair.second, pair.first);
        }
    }

    virtual ~BidirectionalMap()
    {
    }

    const T2& operator[](const T1& value) const
    {
        return Translate(value);
    }

    const T1& operator[](const T2& value) const
    {
        return Translate(value);
    }

    const T2& Translate(const T1& value) const noexcept
    {
        return maptranslation::Translate(m_T1toT2Map, value);
    }

    const T2& Translate(const T1& value, const T2& defaultValue) const noexcept
    {
        return maptranslation::Translate(m_T1toT2Map, value, defaultValue);
    }

    const T1& Translate(const T2& value) const noexcept
    {
        return maptranslation::Translate(m_T2toT1Map, value);
    }

    const T1& Translate(const T2& value, const T1& defaultValue) const noexcept
    {
        return maptranslation::Translate(m_T2toT1Map, value, defaultValue);
    }

    const std::map<T1, T2>& GetMap() const
    {
        return m_T1toT2Map;
    }

    std::vector<T1> GetKeys() const
    {
        std::vector<T1> keys;
        for (auto & keyValuePair : m_T1toT2Map)
        {
            keys.emplace_back(keyValuePair.first);
        }

        return keys;
    }

    std::vector<T2> GetValues() const
    {
        std::vector<T2> values;
        for (auto & keyValuePair : m_T1toT2Map)
        {
            values.emplace_back(keyValuePair.second);
        }

        return values;
    }

private:
    BidirectionalMap()
        : m_T1toT2Map()
        , m_T2toT1Map()
    {}

    std::map<T1, T2> m_T1toT2Map;
    std::map<T2, T1> m_T2toT1Map;
};

} // namespace utility
