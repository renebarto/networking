#pragma once

#include <csignal>
#include <functional>

#if defined(PLATFORM_WINDOWS)
#pragma warning(disable: 4265)
#endif
#include <bitset>
#if defined(PLATFORM_WINDOWS)
#pragma warning(default: 4265)
#endif

namespace osal {

enum class SignalType : std::uint8_t
{
    None = 0,
    Interrupt = SIGINT,
    Illegal = SIGILL,
    FPError = SIGFPE,
    SegmentViolation = SIGSEGV,
    Terminate = SIGTERM,
    Abort = SIGABRT,
};

std::ostream & operator << (std::ostream & stream, SignalType value);

using SignalHandlerFunc = std::function<void (SignalType)>;

void SetSignalHandler(SignalType signal, SignalHandlerFunc handler);
void ResetSignalHandler(SignalType signal);
bool Raise(SignalType signal);

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

inline int SigAction(int /*signum*/, const sigaction_t * /*act*/, sigaction_t * /*oldact*/)
{
    return -1;
}

using sighandler_t = ::_crt_signal_t;

#else

typedef struct ::sigaction sigaction_t;

inline int SigAction(int signum, const sigaction_t *act, sigaction_t *oldact)
{
    return ::sigaction(signum, act, oldact);
}

using sighandler_t = ::sighandler_t;

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

} // namespace osal
