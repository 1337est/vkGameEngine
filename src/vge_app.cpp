// headers
#include "vge_app.hpp"

namespace vge
{
/* Runs the main loop of the application.
 *
 * TODO: Description
 */
void VgeApp::run()
{
    // run until window closes
    while (!m_vgeWindow.shouldClose())
    {
        glfwPollEvents(); // continuously processes and returns received events
    }
}

} // namespace vge
