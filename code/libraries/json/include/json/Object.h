#pragma once

#include <map>
#include <vector>
#include "utility/GenericError.h"
#include "core/Iterator.h"
#include "json/Value.h"

namespace json {

class KVPair
{
public:
    KVPair();
    KVPair(const std::string & key, const ValuePtr & value);

    const std::string & GetKey() const { return m_key; }
    void SetKey(const std::string & value) { m_key = value; }
    const ValuePtr GetValue() const { return m_value; }
    void SetValue(const ValuePtr value) { m_value = value; }
    bool Deserialize(std::istream & stream);
    void Serialize(std::ostream & stream, int indentDepth = 0) const;

private:
    std::string m_key;
    ValuePtr m_value;
};

using KVList = std::map<std::string, ValuePtr>;

class Object : public Value
{
public:
    Object();
    Object(const std::initializer_list<KVPair> & elements);
    virtual ~Object() {};

    class Iterator
    {
    public:
        Iterator(KVList & container)
            : m_begin(container.begin())
            , m_current(m_begin)
            , m_end(container.end())
        {
        }
        bool AtBegin() const { return m_current == m_begin; }
        bool AtEnd() const { return m_current == m_end; }
        void Reset() { m_current = m_begin; }
        KVPair operator++()
        {
            if (!AtEnd())
                ++m_current;
            return operator *();
        }
        KVPair operator++(int)
        {
            KVPair result = operator *();
            if (!AtEnd())
                ++m_current;
            return std::move(result);
        }

        KVPair operator *()
        {
            if (AtEnd())
                tracing::Logging::Throw(__FILE__, __LINE__, __func__, utility::GenericError("Iterator outside scope"));

            return KVPair(m_current->first, m_current->second);
        }

    private:
        KVList::iterator m_begin;
        KVList::iterator m_current;
        KVList::iterator m_end;
    };

    class ConstIterator
    {
    public:
        ConstIterator(const KVList & container)
            : m_begin(container.begin())
            , m_current(m_begin)
            , m_end(container.end())
        {
        }
        bool AtBegin() const { return m_current == m_begin; }
        bool AtEnd() const { return m_current == m_end; }
        void Reset() { m_current = m_begin; }
        KVPair operator++()
        {
            if (!AtEnd())
                ++m_current;
            return operator *();
        }
        KVPair operator++(int)
        {
            KVPair result = operator *();
            if (!AtEnd())
                ++m_current;
            return std::move(result);
        }

        KVPair operator *()
        {
            if (AtEnd())
                tracing::Logging::Throw(__FILE__, __LINE__, __func__, utility::GenericError("Iterator outside scope"));

            return KVPair(m_current->first, m_current->second);
        }

    private:
        KVList::const_iterator m_begin;
        KVList::const_iterator m_current;
        KVList::const_iterator m_end;
    };

    virtual bool Deserialize(std::istream & stream) override;
    virtual void Serialize(std::ostream & stream, int indentDepth = 0, bool indentInitial = true) const override;

    void Clear() { m_elements.clear(); }
    virtual ValueType Type() const override { return ValueType::Object; }
    bool AddPair(const KVPair & pair);
    bool AddPair(const std::string & key, ValuePtr value)
    {
        return AddPair(KVPair(key, value));
    }
    ValuePtr Find(const std::string & name);
    const ValuePtr Find(const std::string & name) const;
    size_t Size() const { return m_elements.size(); }
    Iterator GetIterator() { return Iterator(m_elements); }
    ConstIterator GetIterator() const { return ConstIterator(m_elements); }

private:
    KVList m_elements;
};

template <class T>
ValuePtr CreateObject(const T & obj)
{
    return std::make_shared<Object>(); // Dummy
}

} // namespace json
