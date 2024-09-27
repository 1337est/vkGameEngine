// headers
#include "vge_app.hpp"

#include <iostream>

namespace vge
{
/* Runs the main loop of the application.
 *
 * Continuously polls for window events, keeping the app active until the window
 * is closed. It processes user inputs, window interactions, and other events to
 * ensure the application remains responsive.
 */
void VgeApp::run()
{

    std::cout << "Application is running. Waiting for window events..."
              << std::endl;

    // run until window closes
    while (!m_vgeWindow.shouldClose())
    {
        glfwPollEvents(); // continuously processes and returns received events
    }

    std::cout << "Application loop exited. Window closed." << std::endl;
}

} // namespace vge
