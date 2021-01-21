#pragma once

#include <thread>
#include <string>

namespace osal {

enum class ThreadPriority : uint8_t
{
    IDLE     = 0x00,
    LOWEST   = 0x1F,
    LOW      = 0x3F,
    NORMAL   = 0x7F,
    HIGH     = 0x9F,
    HIGHEST  = 0xBF,
    REALTIME = 0xFF
};

std::string GetThreadNameSelf();

void SetThreadNameSelf(const std::string & name);
void SetThreadName(std::thread & thread, const std::string & name);
bool IsThreadSelf(std::thread & thread);
bool IsThreadAlive(std::thread & thread);

ThreadPriority GetThreadPrioritySelf();
ThreadPriority GetThreadPriority(std::thread & thread);
void SetThreadPrioritySelf(ThreadPriority priority);
void SetThreadPriority(std::thread & thread, ThreadPriority priority);

} // namespace osal
