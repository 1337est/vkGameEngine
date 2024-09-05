#pragma once

// libraries
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vge
{
class VgeCamera
{
public:
    // x-axis is negative left & positive right
    // y-axis is negative up   & positive down
    // z-axis is negative near & positive far
    void setOrthographicProjection(
        float left,   // x: smaller
        float right,  // x: larger
        float top,    // y: smaller
        float bottom, // y: larger
        float near,   // z: smaller
        float far);   // z: larger

    void setPerspectiveProjection(
        float fovy,   // Vertical field of view
        float aspect, // Aspect ratio of window w/h
        float near,   // Near clipping plane
        float far);   // Far clipping plane

    // camera direction
    void setViewDirection(
        glm::vec3 position,
        glm::vec3 direction,
        glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });

    // when you want camera locked on a point in space
    void setViewTarget(
        glm::vec3 position,
        glm::vec3 target,
        glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });

    // specify orientation of the camera using Tate-Bryan Y(1)X(2)Z(3) angles
    void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

    const glm::mat4& getProjection() const
    {
        return m_projectionMatrix;
    }

    const glm::mat4& getView() const
    {
        return m_viewMatrix;
    }

private:
    glm::mat4 m_projectionMatrix{ 1.f };
    glm::mat4 m_viewMatrix{ 1.f };
};
} // namespace vge
