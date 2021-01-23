#pragma once

#include <algorithm>
#include <functional>
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

    void Subscribe(T * observer) override;
    void Unsubscribe(T * observer) override;

    size_t TotalSubscribers() const;

    void ForAll(std::function<void (T *)> action);

protected:
    std::list<T *> m_observers;
    typename std::list<T *>::iterator Find(T * observer);
    Mutex m_guard;
};

template <class T>
size_t Observable<T>::TotalSubscribers() const
{
    return static_cast<size_t>(std::count_if(m_observers.begin(), m_observers.end(), [](const T *) -> bool { return true; }));
}

template <class T>
void Observable<T>::Subscribe(T * observer)
{
    Lock lock(m_guard);
    typename std::list<T *>::iterator it = Find(observer);
    if (it == m_observers.end())
    {
        m_observers.push_back(observer);
    }
}

template <class T>
void Observable<T>::Unsubscribe(T * observer)
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
    // Save in case the subscriber list is changed
    auto tempObservers = m_observers;
    for (auto const & observer : tempObservers)
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
