#pragma once

namespace core
{

template <class T>
class IObservable
{
public:
    virtual ~IObservable() {}

    virtual void Add(T * observer) = 0;
    virtual void Remove(T * observer) = 0;
};

} // namespace core
