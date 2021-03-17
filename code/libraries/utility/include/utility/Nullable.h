//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020 Koninklijke Philips Electronics N.V.
//
// File        : Nullable.h
//
// Namespace   : utility
//
// Class       : Nullable
//
// Description :
//
//------------------------------------------------------------------------------

#pragma once

#include "utility/Deserialization.h"
#include "utility/Serialization.h"

namespace utility {

template <class T>
class Nullable
{
public:
    static const Nullable<T> Null;

    Nullable()
        : m_hasValue(false)
        , m_value()
    {}
    Nullable(const Nullable & other)
        : m_hasValue(other.m_hasValue)
        , m_value(other.m_value)
    {}
    Nullable(Nullable && other)
        : m_hasValue(other.m_hasValue)
        , m_value(std::move(other.m_value))
    {
        other.m_hasValue = false;
    }
    Nullable(const T & value)
        : m_hasValue(true)
        , m_value(value)
    {}
    virtual ~Nullable() {}

    Nullable & operator =(const T & value)
    {
        m_hasValue = true;
        m_value = value;
        return *this;
    }
    Nullable & operator =(const Nullable & other)
    {
        if (this != &other)
        {
            m_hasValue = other.m_hasValue;
            m_value = other.m_value;
        }
        return *this;
    }
    Nullable & operator =(Nullable && other)
    {
        if (this != &other)
        {
            m_hasValue = other.m_hasValue;
            m_value = std::move(other.m_value);
            other.m_hasValue = false;
        }
        return *this;
    }

    void SetNull()
    {
        m_hasValue = false;
        m_value = T {};
    }
    bool HasValue() const { return m_hasValue; }
    T & Value() { return GetValue(); }
    const T & Value() const { return GetValue(); }

protected:
    bool m_hasValue;
    T m_value;

    T & GetValue()
    {
        if (!m_hasValue)
        {
            throw std::logic_error("Nullable has no value to return");

        }
        return m_value;
    }
    const T & GetValue() const
    {
        if (!m_hasValue)
        {
            throw std::logic_error("Nullable has no value to return");

        }
        return m_value;
    }
};

template <class T>
const Nullable<T> Nullable<T>::Null = Nullable<T>();

} // namespace utility

namespace serialization {

template<class T>
bool Deserialize(const std::string & text, utility::Nullable<T> & value)
{
    T parsedValue;
    if (text == "null")
    {
        value.SetNull();
        return true;
    }
    if (Deserialize(text, parsedValue))
    {
        value = parsedValue;
        return true;
    }
    value.SetNull();
    return false;
}

template<class T>
std::string Serialize(const utility::Nullable<T> & value)
{
    if (value.HasValue())
        return Serialize(value.Value());
    return "null";
}

} // namespace serialization
