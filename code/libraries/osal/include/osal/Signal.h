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

#if defined(PLATFORM_WINDOWS)
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
#if _MSC_VER > 1900 // Versions after VS 2015
#pragma warning(disable: 5039)
#endif
    return std::signal(signum, handler);
#if _MSC_VER > 1900 // Versions after VS 2015
#pragma warning(default: 5039)
#endif
}

#else

typedef struct ::sigaction sigaction_t;

inline int sigaction(int signum, const sigaction_t *act, sigaction_t *oldact)
{
    return ::sigaction(signum, act, oldact);
}

using sighandler_t = ::sighandler_t;

inline sighandler_t signal(int signum, sighandler_t handler)
{
    return ::signal(signum, handler);
}

using sigset_t = ::sigset_t;

#endif

class SignalSet
{
public:
    SignalSet();
    int clear();
    int fill();
    int add(int signum);
    int remove(int signum);
    int contains(int signum);
    const sigset_t & get() const;

private:
    sigset_t m_set;
};

enum SignalHow
{
    Block = 0,
    Unblock = 1,
    SetMask = 2,
};

int SetSignalMask(SignalHow how, const sigset_t * set, sigset_t * oldset);

} // namespace signal
} // namespace core