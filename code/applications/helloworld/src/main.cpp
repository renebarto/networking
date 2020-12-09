#include <iostream>

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
    std::cout << "Hello world" << std::endl;    
    return 0;
}
