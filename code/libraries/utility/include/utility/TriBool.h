//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Koninklijke Philips Electronics N.V.
//
// File        : TriBool.h
//
// Namespace   : utility
//
// Class       : TriBool
//
// Description :
//  Replacement of Boost version of tribool.
//
//------------------------------------------------------------------------------

#pragma once

#include <ios>
#include <string>

namespace utility {

class TriBool
{
public:
    enum class Value { False, True, Indeterminate };
    TriBool() noexcept
        : m_value(Value::False)
    {}
    TriBool(bool value) noexcept
        : m_value(value ? Value::True : Value::False)
    {}
    TriBool(Value value) noexcept
        : m_value(value)
    {}

    bool IsTrue() const
    {
        return m_value == Value::True;
    }

    bool IsFalse() const
    {
        return m_value == Value::False;
    }

    bool IsIndeterminate() const
    {
        return m_value == Value::Indeterminate;
    }

    explicit operator bool() const noexcept
    {
        return IsTrue();
    }

    Value m_value;
};

inline TriBool operator!(TriBool x)
{
    return x.m_value == TriBool::Value::False
        ? TriBool(true)
        : x.m_value == TriBool::Value::True
            ? TriBool(false)
            : TriBool(TriBool::Value::Indeterminate);
}

inline TriBool operator &&(TriBool x, TriBool y)
{
    if (static_cast<bool>(!x) || static_cast<bool>(!y))
        return false;
    else if (static_cast<bool>(x) && static_cast<bool>(y))
        return true;
    else
        return TriBool::Value::Indeterminate;
}

inline TriBool operator &&(TriBool x, bool y)
{
    return y ? x : TriBool(false);
}

inline TriBool operator &&(bool x, TriBool y)
{ 
    return x ? y : TriBool(false);
}

inline TriBool operator ||(TriBool x, TriBool y)
{
    if (static_cast<bool>(!x) && static_cast<bool>(!y))
        return false;
    else if (static_cast<bool>(x) || static_cast<bool>(y))
        return true;
    else
        return TriBool::Value::Indeterminate;
}

inline TriBool operator ||(TriBool x, bool y)
{
    return y ? TriBool(true) : x;
}

inline TriBool operator ||(bool x, TriBool y)
{
    return x ? TriBool(true) : y;
}

inline TriBool operator ==(TriBool x, TriBool y)
{
  if (x.IsIndeterminate() || y.IsIndeterminate())
    return TriBool::Value::Indeterminate;
  else
    return (x && y) || (!x && !y);
}

inline TriBool operator ==(TriBool x, bool y)
{
    return x == TriBool(y);
}

inline TriBool operator ==(bool x, TriBool y)
{
    return TriBool(x) == y;
}

inline TriBool operator !=(TriBool x, TriBool y)
{
  if (x.IsIndeterminate() || y.IsIndeterminate())
    return TriBool::Value::Indeterminate;
  else
    return !((x && y) || (!x && !y));
}

inline TriBool operator !=(TriBool x, bool y)
{
    return x != TriBool(y);
}

inline TriBool operator !=(bool x, TriBool y)
{
    return TriBool(x) != y;
}

template<typename T>
std::basic_string<T> default_false_name();

template<>
inline std::basic_string<char> default_false_name<char>()
{ return "false"; }

template<>
inline std::basic_string<wchar_t> default_false_name<wchar_t>()
{ return L"false"; }

template<typename T>
std::basic_string<T> default_true_name();

template<>
inline std::basic_string<char> default_true_name<char>()
{ return "true"; }

template<>
inline std::basic_string<wchar_t> default_true_name<wchar_t>()
{ return L"true"; }

template<typename T> std::basic_string<T> default_indeterminate_name();

template<>
inline std::basic_string<char> default_indeterminate_name<char>()
{ return "indeterminate"; }

template<>
inline std::basic_string<wchar_t> default_indeterminate_name<wchar_t>()
{ return L"indeterminate"; }

template<typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits> &
operator << (std::basic_ostream<CharT, Traits> & out, TriBool x)
{
    if (!x.IsIndeterminate())
    {
        if (static_cast<bool>(x))
            out << default_true_name<CharT>();
        else
            out << default_false_name<CharT>();
    }
    else
    {
        out << default_indeterminate_name<CharT>();
    }
    return out;
}

template<typename CharT, typename Traits>
inline std::basic_istream<CharT, Traits> &
operator >> (std::basic_istream<CharT, Traits> & in, TriBool & x)
{
    typename std::basic_istream<CharT, Traits>::sentry cerberus(in);
    if (cerberus)
    {
        typedef std::basic_string<CharT> string_type;

        string_type falseName = default_false_name<CharT>();
        string_type trueName = default_true_name<CharT>();
        string_type indeterminateName = default_indeterminate_name<CharT>();

        typename string_type::size_type pos = 0;
        bool falseNameOk = true;
        bool trueNameOk = true;
        bool indeterminateNameOk = true;

        // Modeled after the code from Library DR 17
        while ((falseNameOk && pos < falseName.size()) || 
               (trueNameOk && pos < trueName.size()) ||
               (indeterminateNameOk && pos < indeterminateName.size()))
        {
            typename Traits::int_type c = in.get();
            if (c == Traits::eof())
                return in;

            bool matched = false;
            if (falseNameOk && pos < falseName.size())
            {
                if (Traits::eq(Traits::to_char_type(c), falseName[pos]))
                    matched = true;
                else
                    falseNameOk = false;
            }

            if (trueNameOk && pos < trueName.size())
            {
                if (Traits::eq(Traits::to_char_type(c), trueName[pos]))
                    matched = true;
                else
                    trueNameOk = false;
            }

            if (indeterminateNameOk && pos < indeterminateName.size())
            {
                if (Traits::eq(Traits::to_char_type(c), indeterminateName[pos]))
                    matched = true;
                else
                    indeterminateNameOk = false;
            }

            if (matched) { ++pos; }
            if (pos > falseName.size()) falseNameOk = false;
            if (pos > trueName.size())  trueNameOk = false;
            if (pos > indeterminateName.size()) indeterminateNameOk = false;
        }

        if (pos == 0)
            in.setstate(std::ios_base::failbit);
        else
        {
            if (falseNameOk)      x = false;
            else if (trueNameOk)  x = true;
            else if (indeterminateNameOk) x = TriBool::Value::Indeterminate;
            else in.setstate(std::ios_base::failbit);
        }
    }
    return in;
}

} // namespace utility
