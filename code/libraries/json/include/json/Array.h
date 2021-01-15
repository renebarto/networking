#pragma once

#include <map>
#include <vector>
#include "utility/GenericError.h"
#include "core/Iterator.h"
#include "json/Create.h"
#include "json/Value.h"

namespace json {

using ValueList = std::vector<ValuePtr>;
class Array : public Value
{
public:
    Array();
    Array(const std::initializer_list<ValuePtr> & elements);
    virtual ~Array() {};

    class Iterator
    {
    public:
        Iterator(ValueList & container)
        : m_begin(container.begin())
        , m_current(m_begin)
        , m_end(container.end())
        {
        }
        bool AtBegin() const { return m_current == m_begin; }
        bool AtEnd() const { return m_current == m_end; }
        void Reset() { m_current = m_begin; }
        ValuePtr operator++()
        {
            if (!AtEnd())
                ++m_current;
            return operator *();
        }
        ValuePtr operator++(int)
        {
            ValuePtr result = operator *();
            if (!AtEnd())
                ++m_current;
            return std::move(result);
        }

        ValuePtr operator *()
        {
            if (AtEnd())
                tracing::Logging::Throw(__FILE__, __LINE__, __func__, utility::GenericError("Iterator outside scope"));

            return *m_current;
        }

    private:
        ValueList::iterator m_begin;
        ValueList::iterator m_current;
        ValueList::iterator m_end;
    };

    class ConstIterator
    {
    public:
        ConstIterator(const ValueList & container)
        : m_begin(container.begin())
        , m_current(m_begin)
        , m_end(container.end())
        {
        }
        bool AtBegin() const { return m_current == m_begin; }
        bool AtEnd() const { return m_current == m_end; }
        void Reset() { m_current = m_begin; }
        ValuePtr operator++()
        {
            if (!AtEnd())
                ++m_current;
            return operator *();
        }
        ValuePtr operator++(int)
        {
            ValuePtr result = operator *();
            if (!AtEnd())
                ++m_current;
            return std::move(result);
        }

        ValuePtr operator *()
        {
            if (AtEnd())
                tracing::Logging::Throw(__FILE__, __LINE__, __func__, utility::GenericError("Iterator outside scope"));

            return *m_current;
        }

    private:
        ValueList::const_iterator m_begin;
        ValueList::const_iterator m_current;
        ValueList::const_iterator m_end;
    };

    virtual bool Deserialize(std::istream & stream) override;
    virtual void Serialize(std::ostream & stream, int indentDepth = 0, bool indentInitial = true) const override;

    virtual ValueType Type() const override { return ValueType::Array; }

    void Clear() { m_elements.clear(); }
    void AddValue(ValuePtr value);
    size_t Size() const { return m_elements.size(); }
    Iterator GetIterator() { return Iterator(m_elements); }
    ConstIterator GetIterator() const { return ConstIterator(m_elements); }

private:
    ValueList m_elements;
};

template <class T>
ValuePtr CreateArray(const std::vector<T> & values)
{
    auto result = std::make_shared<Array>();
    for (auto const & value : values)
    {
        result->AddValue(CreateValue(value));
    }
    return result;
}

} // namespace json
