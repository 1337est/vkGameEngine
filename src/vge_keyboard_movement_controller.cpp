#include "vge_keyboard_movement_controller.hpp"

#include <GLFW/glfw3.h>

#include <limits>

namespace vge {
/* Moves the game object in the XZ plane based on keyboard input.
 *
 * This method checks the state of specified keys in the GLFW window and
 * applies rotation and translation to the provided game object accordingly.
 * It handles pitch and yaw limits to ensure realistic movement in 3D space,
 * allowing the game object to rotate and move based on user input.
 */
void VgeKeyboardMovementController::moveInPlaneXZ(
    GLFWwindow* window,
    float dt,
    VgeGameObject& gameObject)
{
    glm::vec3 rotate{ 0 };
    if (glfwGetKey(window, m_keys.lookRight) == GLFW_PRESS) {
        rotate.y += 1.f;
    }
    if (glfwGetKey(window, m_keys.lookLeft) == GLFW_PRESS) {
        rotate.y -= 1.f;
    }
    if (glfwGetKey(window, m_keys.lookUp) == GLFW_PRESS) {
        rotate.x += 1.f;
    }
    if (glfwGetKey(window, m_keys.lookDown) == GLFW_PRESS) {
        rotate.x -= 1.f;
    }

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
        gameObject.m_transform.rotation += m_lookSpeed * dt * glm::normalize(rotate);
    }

    // limits pitch values between about +/- 85ish degrees
    gameObject.m_transform.rotation.x = glm::clamp(gameObject.m_transform.rotation.x, -1.5f, 1.5f);
    gameObject.m_transform.rotation.y =
        glm::mod(gameObject.m_transform.rotation.y, glm::two_pi<float>());

    float yaw = gameObject.m_transform.rotation.y;
    const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
    const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
    const glm::vec3 upDir{ 0.f, -1.f, 0.f };

    glm::vec3 moveDir{ 0.f };
    if (glfwGetKey(window, m_keys.moveForward) == GLFW_PRESS) {
        moveDir += forwardDir;
    }
    if (glfwGetKey(window, m_keys.moveBackward) == GLFW_PRESS) {
        moveDir -= forwardDir;
    }
    if (glfwGetKey(window, m_keys.moveRight) == GLFW_PRESS) {
        moveDir += rightDir;
    }
    if (glfwGetKey(window, m_keys.moveLeft) == GLFW_PRESS) {
        moveDir -= rightDir;
    }
    if (glfwGetKey(window, m_keys.moveUp) == GLFW_PRESS) {
        moveDir += upDir;
    }
    if (glfwGetKey(window, m_keys.moveDown) == GLFW_PRESS) {
        moveDir -= upDir;
    }

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
        gameObject.m_transform.translation += m_moveSpeed * dt * glm::normalize(moveDir);
    }
}
} // namespace vge
