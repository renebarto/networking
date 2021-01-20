#pragma once

namespace core
{

template <class T>
class IObservable
{
public:
    virtual ~IObservable() {}

    virtual void Subscribe(T * observer) = 0;
    virtual void Unsubscribe(T * observer) = 0;
};

} // namespace core
