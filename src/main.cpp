#include "vge_app.hpp"

#include <cstdlib>   // EXIT_FAILURE & EXIT_SUCCESS macros
#include <exception> // std::exception e.what()
#include <iostream>  // std::cerr

/* Entry point for the VgeApp application.
 *
 * Initializes the VgeApp instance and runs the main application loop. If an
 * exception occurs during execution, it catches the exception, logs the error
 * message, and returns a failure status.
 */
int main()
{
    vge::VgeApp app{};

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
