#pragma once

#include "utility/GenericError.h"
#include "tracing/Tracing.h"

namespace core {

template<class Container>
class Iterator
{
public:
    using Element = typename Container::value_type;
    Iterator(Container & container)
        : m_begin(container.begin())
        , m_current(m_begin)
        , m_end(container.end())
    {
    }
    bool AtBegin() const { return m_current == m_begin; }
    bool AtEnd() const { return m_current == m_end; }
    void Reset() { m_current = m_begin; }
    Element & operator++()
    {
        if (!AtEnd())
            ++m_current;
        return operator *();
    }
    Element & operator++(int)
    {
        Element & result = operator *();
        if (!AtEnd())
            ++m_current;
        return result;
    }

    Element * operator ->()
    {
        if ((m_current < m_begin) || (m_current >= m_end))
            tracing::Tracer::Throw(__FILE__, __LINE__, __FUNCTION__, utility::GenericError("Iterator outside range"));

        return m_current;
    }
    Element & operator *()
    {
        if ((m_current < m_begin) || (m_current >= m_end))
            tracing::Tracer::Throw(__FILE__, __LINE__, __FUNCTION__, utility::GenericError("Iterator outside range"));

        return *m_current;
    }

private:
    typename Container::iterator m_begin;
    typename Container::iterator m_current;
    typename Container::iterator m_end;
};

template<class Container>
class ConstIterator
{
public:
    using Element = typename Container::value_type;
    ConstIterator(const Container & container)
        : m_begin(container.begin())
        , m_current(m_begin)
        , m_end(container.end())
    {
    }
    bool AtBegin() const { return m_current == m_begin; }
    bool AtEnd() const { return m_current == m_end; }
    void Reset() { m_current = m_begin; }
    const Element & operator++()
    {
        if (!AtEnd())
            ++m_current;
        return operator *();
    }
    const Element & operator++(int)
    {
        const Element & result = operator *();
        if (!AtEnd())
            ++m_current;
        return result;
    }

    const Element * operator ->()
    {
        if ((m_current < m_begin) || (m_current >= m_end))
            tracing::Tracer::Throw(__FILE__, __LINE__, __FUNCTION__, utility::GenericError("Iterator outside range"));

        return m_current;
    }
    const Element & operator *() const
    {
        if ((m_current < m_begin) || (m_current >= m_end))
            tracing::Tracer::Throw(__FILE__, __LINE__, __FUNCTION__, utility::GenericError("Iterator outside range"));

        return *m_current;
    }

private:
    typename Container::const_iterator m_begin;
    typename Container::const_iterator m_current;
    typename Container::const_iterator m_end;
};

} // namespace core