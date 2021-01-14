#include <iostream>
#include "alsa/asoundlib.h"

bool CheckArgs(int argc, char *argv[]);
void Usage(int argc, char *argv[]);
int Run(int argc, char *argv[]);

int main(int argc, char * argv[])
{
    try
    {
        if (!CheckArgs(argc, argv))
        {
            Usage(argc, argv);
            return 1;
        }
        return Run(argc, argv);
    }
    catch (std::exception & e)
    {
        std::cerr << "Unhandled Exception: " << e.what() << std::endl;
    }
}

bool CheckArgs(int argc, char *argv[])
{
    std::cout << "main(" << argc << ", [";
    for (int i = 0; i < argc; ++i)
    {
        std::cout << argv[i];
        if (i < argc - 1)
            std::cout << ", ";
    }
    std::cout << "])" << std::endl;
    // if (argc < 2)
    // {
    //     return false;
    // }
    return true;
}

void Usage(int /*argc*/, char *argv[])
{
    std::cerr << "Usage: " << argv[0] << " <descriptor>" << std::endl;
}

int Run(int /*argc*/, char * /*argv*/[])
{
    char **hints;
    /* Enumerate sound devices */
    int err = snd_device_name_hint(-1, "pcm", (void***)&hints);
    if (err != 0)
        return 1;//Error! Just return

    char** n = hints;
    while (*n != NULL)
    {

        char *name = snd_device_name_get_hint(*n, "NAME");

        if (name != NULL && 0 != strcmp("null", name)) {
            std::cout << name << std::endl;
            //Copy name to another buffer and then free it
            free(name);
        }
        n++;
    }//End of while

    //Free hint buffer too
    snd_device_name_free_hint((void**)hints);
    return 0;
}
