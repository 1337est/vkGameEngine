// headers
#include "vge_app.hpp"

// std
#include <cstdlib>   // EXIT_FAILURE & EXIT_SUCCESS macros
#include <exception> // std::exception e.what()
#include <iostream>  // std::cerr

int main()
{
    vge::VgeApp app{};

    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
