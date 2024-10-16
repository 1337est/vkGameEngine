#pragma once

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vge {
class VgeCamera {
public:
    void setOrthographicProjectionMatrix(
        float left,   // x: smaller
        float right,  // x: larger
        float top,    // y: smaller
        float bottom, // y: larger
        float near,   // z: smaller
        float far);   // z: larger

    void setPerspectiveProjectionMatrix(
        float fovy,   // Vertical field of view
        float aspect, // Aspect ratio of window w/h
        float near,   // Near clipping plane
        float far);   // Far clipping plane

    void setViewDirectionMatrix(
        glm::vec3 position,
        glm::vec3 direction,
        glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });

    void setViewTargetDirectionMatrix(
        glm::vec3 position,
        glm::vec3 target,
        glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });

    void setViewYXZMatrix(glm::vec3 position, glm::vec3 rotation);

    const glm::mat4& getProjectionMatrix() const;
    const glm::mat4& getViewMatrix() const;
    const glm::mat4& getInverseViewMatrix() const;

private:
    glm::mat4 m_projectionMatrix{ 1.f };
    glm::mat4 m_viewMatrix{ 1.f };
    glm::mat4 m_inverseViewMatrix{ 1.f };
};
} // namespace vge
