#include "osal/Signal.h"

namespace osal {
namespace signal {

#if defined(PLATFORM_WINDOWS)

static sigset_t signalMask;

SignalSet::SignalSet()
    : m_set()
{
    clear();
}

int SignalSet::clear()
{
    m_set.set.reset();
    return 0;
}
int SignalSet::fill()
{
    m_set.set.set();
    return 0;
}
int SignalSet::add(int signum)
{
    if ((signum < 0) || (static_cast<size_t>(signum) >= m_set.set.size()))
        return -1;
    m_set.set.set(static_cast<size_t>(signum));
    return 0;
}
int SignalSet::remove(int signum)
{
    if ((signum < 0) || (static_cast<size_t>(signum) >= m_set.set.size()))
        return -1;
    m_set.set.reset(static_cast<size_t>(signum));
    return 0;
}
int SignalSet::contains(int signum)
{
    if ((signum < 0) || (static_cast<size_t>(signum) >= m_set.set.size()))
        return -1;
    return (m_set.set.test(static_cast<size_t>(signum))) ? 1 : 0;
}
const sigset_t & SignalSet::get() const
{
    return m_set;
}

int SetSignalMask(SignalHow how, const sigset_t * set, sigset_t * oldset)
{
    if (oldset != nullptr)
        *oldset = signalMask;
    if (set == nullptr)
        return 0;
    switch (how)
    {
    case SignalHow::Block:
        signalMask.set |= set->set;
        break;
    case SignalHow::Unblock:
        signalMask.set &= (~set->set);
        break;
    case SignalHow::SetMask:
        signalMask = *set;
        break;
    default:
        return -1;
    }
    return 0;
}

#else

SignalSet::SignalSet()
    : m_set()
{
    clear();
}

int SignalSet::clear()
{
    return ::sigemptyset(&m_set);
}

int SignalSet::fill()
{
    return ::sigfillset(&m_set);
}

int SignalSet::add(int signum)
{
    return ::sigaddset(&m_set, signum);
}

int SignalSet::remove(int signum)
{
    return ::sigdelset(&m_set, signum);
}

int SignalSet::contains(int signum)
{
    return ::sigismember(&m_set, signum);
}

const sigset_t & SignalSet::get() const
{
    return m_set;
}

int SetSignalMask(SignalHow how, const sigset_t * set, sigset_t * oldset)
{
    return ::pthread_sigmask(how, set, oldset);
}

#endif

} // namespace signal
} // namespace osal
