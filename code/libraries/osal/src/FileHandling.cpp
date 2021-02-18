#include "osal/FileHandling.h"

#include <cstdlib>

//TICS -POR#021 Platform dependent
#if defined(PLATFORM_WINDOWS)
#if _MSC_VER > 1900 // Versions after VS 2015
#pragma warning(disable: 5039) //TICS !POR#018 !POR#037
#endif
#include <windows.h>
#if _MSC_VER > 1900 // Versions after VS 2015
#pragma warning(default: 5039) //TICS !POR#018 !POR#037
#endif
#include <io.h>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

FileDescriptor GetFileDescriptor(FILE * file)
{
    return ::_fileno(file);
}
const char * GetEnvironment(const char * name)
{
    static char buffer[4096];
    if (::GetEnvironmentVariableA(name, buffer, sizeof(buffer)) == 0)
    {
        return nullptr;
    }
    return buffer;
}
bool IsTTY(int fd)
{
    return ::_isatty(fd) != 0;
}

#else

FileDescriptor GetFileDescriptor(FILE * file)
{
    return ::fileno(file);
}
const char * GetEnvironment(const char * name)
{
    return ::getenv(name);
}
bool IsTTY(int fd)
{
    return ::isatty(fd) != 0;
}

#endif
//TICS +POR#021
