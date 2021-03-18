#include "osal/Signal.h"

#include <iostream>
#include <map>

namespace osal {

static std::map<SignalType, const char *> s_signalLookup {
    { SignalType::None, "None" },
    { SignalType::Interrupt, "Interrupt" },
    { SignalType::Illegal, "Illegal" },
    { SignalType::FPError, "FPError" },
    { SignalType::SegmentViolation, "SegmentViolation" },
    { SignalType::Terminate, "Terminate" },
    { SignalType::Abort, "Abort" },
};

std::ostream & operator << (std::ostream & stream, SignalType value)
{
    auto it = s_signalLookup.find(value);
    if (it != s_signalLookup.end())
    {
        stream << it->second;
    }
    else
    {
        stream << "Unknown: " << static_cast<int>(value);
    }
    return stream;
}

class SignalHandlerRegistry
{
private:
    static std::map<SignalType, SignalHandlerFunc> m_signalMap;
    static std::map<SignalType, SignalHandlerFunc> m_defaultSignalMap;

public:
    SignalHandlerRegistry()
    {
        SetupSignalHandler(SignalType::Interrupt);
        SetupSignalHandler(SignalType::Illegal);
        SetupSignalHandler(SignalType::FPError);
        SetupSignalHandler(SignalType::SegmentViolation);
        SetupSignalHandler(SignalType::Terminate);
        SetupSignalHandler(SignalType::Abort);
    }

    void SetupSignalHandler(SignalType signal)
    {
        auto oldSignal = std::signal(static_cast<int>(signal), SignalHandlerOS);
        if (oldSignal == SIG_DFL)
        {
            m_defaultSignalMap[signal] = std::bind(&SignalHandlerRegistry::SignalHandlerDefault, this, std::placeholders::_1);
        }
        else if (oldSignal == SIG_IGN)
        {
            m_defaultSignalMap[signal] = std::bind(&SignalHandlerRegistry::SignalHandlerIgnore, this, std::placeholders::_1);
        }
        else
        {
            m_defaultSignalMap[signal] = std::bind(&SignalHandlerRegistry::SignalHandlerIgnore, this, std::placeholders::_1);
        }
        ResetSignalHandler(signal);
    }    
    void SetSignalHandler(SignalType signal, SignalHandlerFunc handler)
    {
        m_signalMap[signal] = handler;
    }
    void ResetSignalHandler(SignalType signal)
    {
        m_signalMap[signal] = m_defaultSignalMap[signal];
    }
    void SignalHandlerDefault(SignalType signal)
    {
        switch (signal)
        {
            case SignalType::Interrupt:
            case SignalType::Illegal:
            case SignalType::FPError:
            case SignalType::SegmentViolation:
            case SignalType::Terminate:
            case SignalType::Abort:
                exit(static_cast<int>(signal));
            case SignalType::None:
                break;
            default:
                break;
        }
    }

    void SignalHandlerIgnore(SignalType /*signal*/)
    {
    }

    static void SignalHandlerOS(int signalOS) noexcept
    {
        SignalType signal = static_cast<SignalType>(signalOS);
        auto & handler = m_signalMap[signal];
        if (!handler)
        {
            std::cout << "Signal not supported, no entry in map! Terminating..." << std::endl;
            std::terminate();
        }
        handler(signal);
    }

};

std::map<SignalType, SignalHandlerFunc> SignalHandlerRegistry::m_signalMap {};
std::map<SignalType, SignalHandlerFunc> SignalHandlerRegistry::m_defaultSignalMap {};
static SignalHandlerRegistry s_registry;

void SetSignalHandler(SignalType signal, SignalHandlerFunc handler)
{
    s_registry.SetSignalHandler(signal, handler);
}
void ResetSignalHandler(SignalType signal)
{
    s_registry.ResetSignalHandler(signal);
}
bool Raise(SignalType signal)
{
    return ::raise(static_cast<int>(signal)) == 0;
}

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

} // namespace osal
