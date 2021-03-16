#pragma once

#include <set>

namespace tracing
{

template<class Category>
class CategorySet
{
private:
    std::set<Category> m_set;
    using MaskType = typename std::underlying_type<Category>::type;

public:
    CategorySet()
        : m_set()
    {}

    CategorySet(MaskType mask)
        : m_set()
    {
        set(static_cast<Category>(mask));
    }

    CategorySet(Category traceCategory)
        : m_set()
    {
        set(traceCategory);
    }

    void clear()
    {
        m_set.clear();
    }

    void set(Category traceCategory)
    {
        MaskType tmpMask = static_cast<MaskType>(1) << (sizeof(MaskType) * 8 - 1);
        while (tmpMask != 0)
        {
            if ((static_cast<MaskType>(traceCategory) & tmpMask) != 0)
                m_set.emplace(static_cast<Category>(tmpMask));
            tmpMask >>= 1;
        }
    }

    void reset(Category traceCategory)
    {
        m_set.erase(traceCategory);
    }

    bool is_set(Category traceCategory) const
    {
        return m_set.find(traceCategory) != m_set.end();
    }

    CategorySet & operator |= (Category traceCategory)
    {
        set(traceCategory);
        return *this;
    }
};

template<class Category>
inline CategorySet<Category> operator | (CategorySet<Category> lhs, Category rhs)
{
    CategorySet<Category> newSet = lhs;
    newSet |= rhs;
    return newSet;
}

template<class Category>
inline CategorySet<Category> operator | (Category lhs, Category rhs)
{
    CategorySet<Category> newSet = lhs;
    newSet |= rhs;
    return newSet;
}

} // namespace tracing

