#pragma once

#include <csignal>

#if defined(PLATFORM_WINDOWS)
#pragma warning(disable: 4265)
#endif
#include <bitset>
#if defined(PLATFORM_WINDOWS)
#pragma warning(default: 4265)
#endif

namespace osal {
namespace signal {

union sigval
{
    int    sival_int;       // Integer signal value.
    void  *sival_ptr;       // Pointer signal value.
};

using pid_t = int;
using uid_t = int;

struct siginfo_t
{
    int           si_signo; // Signal number.
    int           si_errno; // If non - zero, an errno value associated with this signal, as defined in <errno.h>.

    int           si_code;  // Signal code.

    pid_t         si_pid;   // Sending process ID.
    uid_t         si_uid;   // Real user ID of sending process.
    void         *si_addr;  // Address of faulting instruction.
    int           si_status;// Exit value or signal.
    long          si_band;  // Band event for SIGPOLL.

    union sigval  si_value; // Signal value.
};

struct sigset_t
{
    std::bitset<320> set;
};

struct sigaction_t
{
    void(*sa_handler)(int);
    void(*sa_sigaction)(int, siginfo_t *, void *);
    sigset_t   sa_mask;
    int        sa_flags;
    void(*sa_restorer)(void);
};

inline int sigaction(int /*signum*/, const sigaction_t * /*act*/, sigaction_t * /*oldact*/)
{
    return -1;
}

using sighandler_t = ::_crt_signal_t;

inline sighandler_t signal(int signum, sighandler_t handler)
{
#if defined(PLATFORM_WINDOWS)
#if _MSC_VER > 1900 // Versions after VS 2015
#pragma warning(disable: 5039)
#endif
#endif
    return std::signal(signum, handler);
#if defined(PLATFORM_WINDOWS)
#if _MSC_VER > 1900 // Versions after VS 2015
#pragma warning(default: 5039)
#endif
#endif
}

class SignalSet
{
public:
    SignalSet()
            : _set()
    {
        clear();
    }
    int clear()
    {
        _set.set.reset();
        return 0;
    }
    int fill()
    {
        _set.set.set();
        return 0;
    }
    int add(int signum)
    {
        if ((signum < 0) || (static_cast<size_t>(signum) >= _set.set.size()))
            return -1;
        _set.set.set(static_cast<size_t>(signum));
        return 0;
    }
    int remove(int signum)
    {
        if ((signum < 0) || (static_cast<size_t>(signum) >= _set.set.size()))
            return -1;
        _set.set.reset(static_cast<size_t>(signum));
        return 0;
    }
    int contains(int signum)
    {
        if ((signum < 0) || (static_cast<size_t>(signum) >= _set.set.size()))
            return -1;
        return (_set.set.test(static_cast<size_t>(signum))) ? 1 : 0;
    }
    const sigset_t & get() const
    {
        return _set;
    }

private:
    sigset_t _set;
};

enum SignalHow
{
    Block = 0,
    Unblock = 1,
    SetMask = 2,
};

int SetSignalMask(SignalHow how, const sigset_t *set, sigset_t *oldset);

} // namespace signal
} // namespace core