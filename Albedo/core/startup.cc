#include "startup.h"

#include <stdexcept>

int main(int argc, char* argv[])
{
    try
    {
        Albedo::APP::instance().run();
    }
    catch (std::exception& e)
    {
        Albedo::log::critical("[Albedo] Unsolved  Exception {}", e.what());
    }

    return 0;
}