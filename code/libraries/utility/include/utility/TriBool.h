#pragma once

#include <ios>
#include <string>

namespace utility {

namespace internal {

struct indeterminate_type
{
};

} // namespace internal

class TriBool;

typedef bool (*indeterminate_keyword_t)(TriBool, internal::indeterminate_type);

inline bool
indeterminate(TriBool x,
              internal::indeterminate_type dummy = internal::indeterminate_type());

class TriBool
{
public:
    enum value_t { false_value, true_value, indeterminate_value };
    TriBool() noexcept
        : value(false_value)
    {}
    TriBool(bool initial_value) noexcept
        : value(initial_value ? true_value : false_value)
    {}
    TriBool(indeterminate_keyword_t) noexcept
        : value(indeterminate_value)
    {}
    TriBool(value_t initial_value) noexcept
        : value(initial_value)
    {}

    bool IsTrue() const
    {
        return value == true_value;
    }

    bool IsFalse() const
    {
        return value == false_value;
    }

    bool IsIndeterminate() const
    {
        return value == indeterminate_value;
    }

    explicit operator bool() const noexcept
    {
        return IsTrue();
    }

    value_t value;
};

inline bool indeterminate(TriBool x, internal::indeterminate_type)
{
    return x.IsIndeterminate();
}

inline TriBool operator!(TriBool x)
{
    return x.value == TriBool::false_value
        ? TriBool(true)
        : x.value == TriBool::true_value
            ? TriBool(false)
            : TriBool(indeterminate);
}

inline TriBool operator &&(TriBool x, TriBool y)
{
    if (static_cast<bool>(!x) || static_cast<bool>(!y))
        return false;
    else if (static_cast<bool>(x) && static_cast<bool>(y))
        return true;
    else
        return indeterminate;
}

inline TriBool operator &&(TriBool x, bool y)
{
    return y ? x : TriBool(false);
}

inline TriBool operator &&(bool x, TriBool y)
{ 
    return x ? y : TriBool(false);
}

inline TriBool operator &&(indeterminate_keyword_t, TriBool x)
{ 
    return !x ? TriBool(false) : TriBool(indeterminate);
}

inline TriBool operator &&(TriBool x, indeterminate_keyword_t)
{
    return !x ? TriBool(false) : TriBool(indeterminate);
}

inline TriBool operator ||(TriBool x, TriBool y)
{
    if (static_cast<bool>(!x) && static_cast<bool>(!y))
        return false;
    else if (static_cast<bool>(x) || static_cast<bool>(y))
        return true;
    else
        return indeterminate;
}

inline TriBool operator ||(TriBool x, bool y)
{
    return y ? TriBool(true) : x;
}

inline TriBool operator ||(bool x, TriBool y)
{
    return x ? TriBool(true) : y;
}

inline TriBool operator ||(indeterminate_keyword_t, TriBool x)
{
    return x ? TriBool(true) : TriBool(indeterminate);
}

inline TriBool operator ==(TriBool x, TriBool y)
{
  if (indeterminate(x) || indeterminate(y))
    return indeterminate;
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

inline TriBool operator ==(indeterminate_keyword_t, TriBool x)
{
    return TriBool(indeterminate) == x;
}

inline TriBool operator ==(TriBool x, indeterminate_keyword_t)
{
    return TriBool(indeterminate) == x;
}

inline TriBool operator !=(TriBool x, TriBool y)
{
  if (indeterminate(x) || indeterminate(y))
    return indeterminate;
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

inline TriBool operator !=(indeterminate_keyword_t, TriBool x)
{
    return TriBool(indeterminate) != x;
}

inline TriBool operator !=(TriBool x, indeterminate_keyword_t)
{
    return x != TriBool(indeterminate);
}

template<typename T> std::basic_string<T> default_false_name();

template<>
inline std::basic_string<char> default_false_name<char>()
{ return "false"; }

template<>
inline std::basic_string<wchar_t> default_false_name<wchar_t>()
{ return L"false"; }

template<typename T> std::basic_string<T> default_true_name();

template<>
inline std::basic_string<char> default_true_name<char>()
{ return "true"; }

template<>
inline std::basic_string<wchar_t> default_true_name<wchar_t>()
{ return L"true"; }

template<typename T> std::basic_string<T> get_default_indeterminate_name();

template<>
inline std::basic_string<char> get_default_indeterminate_name<char>()
{ return "indeterminate"; }

template<>
inline std::basic_string<wchar_t> get_default_indeterminate_name<wchar_t>()
{ return L"indeterminate"; }

template<typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> & out, TriBool x)
{
    if (!indeterminate(x))
    {
        if (static_cast<bool>(x))
            out << default_true_name<CharT>();
        else
            out << default_false_name<CharT>();
    }
    else
    {
        out << get_default_indeterminate_name<CharT>();
    }
    return out;
}

template<typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> & out, 
           bool (*)(TriBool, internal::indeterminate_type))
{ 
    return out << TriBool(indeterminate);
} 

template<typename CharT, typename Traits>
inline std::basic_istream<CharT, Traits> &
operator>>(std::basic_istream<CharT, Traits> & in, TriBool & x)
{
    typename std::basic_istream<CharT, Traits>::sentry cerberus(in);
    if (cerberus) {
        typedef std::basic_string<CharT> string_type;

        string_type falsename = default_false_name<CharT>();
        string_type truename = default_true_name<CharT>();
        string_type othername = get_default_indeterminate_name<CharT>();

        typename string_type::size_type pos = 0;
        bool falsename_ok = true, truename_ok = true, othername_ok = true;

        // Modeled after the code from Library DR 17
        while ((falsename_ok && pos < falsename.size()) || 
               (truename_ok && pos < truename.size()) ||
               (othername_ok && pos < othername.size()))
        {
            typename Traits::int_type c = in.get();
            if (c == Traits::eof())
                return in;

            bool matched = false;
            if (falsename_ok && pos < falsename.size())
            {
                if (Traits::eq(Traits::to_char_type(c), falsename[pos]))
                    matched = true;
                else
                    falsename_ok = false;
            }

            if (truename_ok && pos < truename.size())
            {
                if (Traits::eq(Traits::to_char_type(c), truename[pos]))
                    matched = true;
                else
                    truename_ok = false;
            }

            if (othername_ok && pos < othername.size())
            {
                if (Traits::eq(Traits::to_char_type(c), othername[pos]))
                    matched = true;
                else
                    othername_ok = false;
            }

            if (matched) { ++pos; }
            if (pos > falsename.size()) falsename_ok = false;
            if (pos > truename.size())  truename_ok = false;
            if (pos > othername.size()) othername_ok = false;
        }

        if (pos == 0)
            in.setstate(std::ios_base::failbit);
        else
        {
            if (falsename_ok)      x = false;
            else if (truename_ok)  x = true;
            else if (othername_ok) x = indeterminate;
            else in.setstate(std::ios_base::failbit);
        }
    }
    return in;
}

} // namespace utility
