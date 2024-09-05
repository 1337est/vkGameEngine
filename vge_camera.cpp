#include "vge_camera.hpp"

// std
#include <cassert>
#include <limits>

namespace vge
{
void VgeCamera::setOrthographicProjection(
    float left,
    float right,
    float top,
    float bottom,
    float near,
    float far)
{
    m_projectionMatrix = glm::mat4{ 1.0f };
    m_projectionMatrix[0][0] = 2.f / (right - left);
    m_projectionMatrix[1][1] = 2.f / (bottom - top);
    m_projectionMatrix[2][2] = 1.f / (far - near);
    m_projectionMatrix[3][0] = -(right + left) / (right - left);
    m_projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
    m_projectionMatrix[3][2] = -near / (far - near);
}

void VgeCamera::setPerspectiveProjection(
    float fovy,
    float aspect,
    float near,
    float far)
{
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
    const float tanHalfFovy = static_cast<float>(tan(fovy / 2.f));
    m_projectionMatrix = glm::mat4{ 0.0f };
    m_projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
    m_projectionMatrix[1][1] = 1.f / (tanHalfFovy);
    m_projectionMatrix[2][2] = far / (far - near);
    m_projectionMatrix[2][3] = 1.f;
    m_projectionMatrix[3][2] = -(far * near) / (far - near);
}

void VgeCamera::setViewDirection(
    glm::vec3 position,
    glm::vec3 direction,
    glm::vec3 up)
{
    // Creating orthonormal basis vectors: unit length 1, orthogonal/normalized
    const glm::vec3 w{ glm::normalize(direction) }; // direction is along z-axis
    const glm::vec3 u{ glm::normalize(glm::cross(w, up)) }; // along x-axis
    const glm::vec3 v{ glm::cross(w, u) }; // cross wu is normalized to y-axis

    /* Uses basis vectors to construct a rotation matrix, combined with a
     * translation matrix from the camera's position back to the origin
     * Rotation 4x4 matrix:
     * [col0row0, col1row0, col2row0, col3row0]
     * [col0row1, col1row1, col2row1, col3row1]
     * [col0row2, col1row2, col2row2, col3row2]
     * [col0row3, col1row3, col2row3, col3row3]
    Rotation matrix (R):
        u.x, u.y, u.z, 0
        v.x, v.y, v.z, 0
        w.x, w.y, w.z, 0
        0,   0,   0,   1
    Translation matrix (T):
        1,  0,  0,  -p.x
        0,  1,  0,  -p.y
        0,  0,  1,  -p.z
        0,  0,  0,  1
    View matrix (V): V = R * T
        u.x, u.y, u.z, -dot(u, position)
        v.x, v.y, v.z, -dot(v, position)
        w.x, w.y, w.z, -dot(w, position)
        0,   0,   0,   1
     */
    m_viewMatrix = glm::mat4{ 1.f }; // set diagnal to identity matrix
    m_viewMatrix[0][0] = u.x;        // col0row0
    m_viewMatrix[1][0] = u.y;        // col1row0
    m_viewMatrix[2][0] = u.z;        // col2row0
    m_viewMatrix[0][1] = v.x;        // col0row1
    m_viewMatrix[1][1] = v.y;        // col1row1
    m_viewMatrix[2][1] = v.z;        // col2row1
    m_viewMatrix[0][2] = w.x;        // col0row2
    m_viewMatrix[1][2] = w.y;        // col1row2
    m_viewMatrix[2][2] = w.z;        // col2row2
    m_viewMatrix[3][0] = -glm::dot(u, position); // col3row0
    m_viewMatrix[3][1] = -glm::dot(v, position); // col3row1
    m_viewMatrix[3][2] = -glm::dot(w, position); // col3row2
}

void VgeCamera::setViewTarget(
    glm::vec3 position,
    glm::vec3 target,
    glm::vec3 up)
{
    setViewDirection(position, target - position, up);
}

void VgeCamera::setViewYXZ(glm::vec3 position, glm::vec3 rotation)
{
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3),
                       (c2 * s3),
                       (c1 * s2 * s3 - c3 * s1) };
    const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3),
                       (c2 * c3),
                       (c1 * c3 * s2 + s1 * s3) };
    const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
    m_viewMatrix = glm::mat4{ 1.f };
    m_viewMatrix[0][0] = u.x;
    m_viewMatrix[1][0] = u.y;
    m_viewMatrix[2][0] = u.z;
    m_viewMatrix[0][1] = v.x;
    m_viewMatrix[1][1] = v.y;
    m_viewMatrix[2][1] = v.z;
    m_viewMatrix[0][2] = w.x;
    m_viewMatrix[1][2] = w.y;
    m_viewMatrix[2][2] = w.z;
    m_viewMatrix[3][0] = -glm::dot(u, position);
    m_viewMatrix[3][1] = -glm::dot(v, position);
    m_viewMatrix[3][2] = -glm::dot(w, position);
}
} // namespace vge
