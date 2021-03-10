#include <iostream>
#include "App.h"

int main(int argc, char * argv[])
{
    int result {};
    Application app(argc, argv);
    try
    {
        if (!app.CheckArgs())
        {
            app.Usage();
            result = 1;
        }
        else
        {
            result = app.Run();
        }
    }
    catch (std::exception & e)
    {
        std::cerr << "Unhandled Exception: " << e.what() << std::endl;
        result = 2;
    }
    return result;
}
