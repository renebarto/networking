#pragma once

#include <algorithm>
#include <list>
#include <mutex>
#include "core/IObservable.h"

namespace core
{

template <class T>
class Observable : public IObservable<T>
{
public:
    typedef std::recursive_mutex Mutex;
    typedef std::lock_guard<Mutex> Lock;

    void Add(T * observer) override;
    void Remove(T * observer) override;

    size_t Count() const;

    void ForAll(std::function<void (T *)> action);

protected:
    std::list<T *> m_observers;
    typename std::list<T *>::iterator Find(T * observer);
    Mutex m_guard;
};

template <class T>
size_t Observable<T>::Count() const
{
    return static_cast<size_t>(std::count_if(m_observers.begin(), m_observers.end(), [](const T *) -> bool { return true; }));
}

template <class T>
void Observable<T>::Add(T * observer)
{
    Lock lock(m_guard);
    typename std::list<T *>::iterator it = Find(observer);
    if (it == m_observers.end())
    {
        m_observers.push_back(observer);
    }
}

template <class T>
void Observable<T>::Remove(T * observer)
{
    Lock lock(m_guard);
    typename std::list<T *>::iterator it = Find(observer);
    if (it != m_observers.end())
    {
        m_observers.erase(it);
    }
}

template<class T>
void Observable<T>::ForAll(std::function<void (T *)> action)
{
    for (auto const & observer : m_observers)
    {
        action(observer);
    }
}

template <class T>
typename std::list<T *>::iterator Observable<T>::Find(T * observer)
{
    Lock lock(m_guard);
    typename std::list<T *>::iterator it = m_observers.begin();
    while (it != m_observers.end())
    {
        if (*it == observer)
            break;
        ++it;
    }
    return it;
}

} // namespace core
