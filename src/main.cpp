// hdrs
#include "vge_app.hpp"
// libs
// stds
#include <cstdlib>   // EXIT_FAILURE & EXIT_SUCCESS macros
#include <exception> // std::exception e.what()
#include <iostream>  // std::cerr

/* Entry point for the Vulkan Game Engine Application
 *
 * Initializes the VgeApp instance and runs the main application loop. If an
 * exception occurs during execution, it catches the exception, logs the error
 * message, and returns a failure status.
 */

int main()
{
    vge::VgeApp app{};

    try
    {

        std::cout << "Starting Vulkan Game Engine Application..."
                  << std::endl; // Log start message

        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "Application closed successfully."
              << std::endl; // Log successful exit

    return EXIT_SUCCESS;
}
