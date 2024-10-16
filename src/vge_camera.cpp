#include "vge_camera.hpp"

#include <cassert>
#include <limits>

namespace vge {

/* Sets the Vulkan Canonical View Volume using an Orthographic Projection Matrix
 *
 * OrthographicProjectionMatrix = ScaleMatrix * TranslationMatrix
 *
 * The orthographic projection defines a 2D view from a 3D space using clipping
 * planes.
 */
void VgeCamera::setOrthographicProjectionMatrix(
    float left,
    float right,
    float top,
    float bottom,
    float near,
    float far)
{
    // glm is col major, so col is defined 1st
    m_projectionMatrix = glm::mat4{ 1.0f }; // set to identity matrix
    // Scaling the Vulkan Canonical View to the Orthographic View
    // numerator = Vulkan Canonical View volume dimensions
    // denominator = Orthographic View volume dimensions
    m_projectionMatrix[0][0] = 2.f / (right - left);
    m_projectionMatrix[1][1] = 2.f / (bottom - top);
    m_projectionMatrix[2][2] = 1.f / (far - near);
    // Translation to center the near plane to the origin
    m_projectionMatrix[3][0] = -(right + left) / (right - left);
    m_projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
    m_projectionMatrix[3][2] = -near / (far - near);
}

/* Sets the Vulkan Canonical View Volume using a Perspective Projection Matrix.
 *
 * PerspectiveProjectionMatrix = OrthographicProjectionMatrix *
 * PerspectiveMatrix
 *
 * The perspective projection defines a matrix based on the vertical field of
 * view, aspect ratio of the window, and clipping planes.
 */
void VgeCamera::setPerspectiveProjectionMatrix(float fovy, float aspect, float near, float far)
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

/* Sets the camera's View Direction using a Rotation and Translation Matrix.
 *
 * The view direction is set using the camera's current position, facing
 * direction, and up vector to form an Orthonormal Basis with u, v, and w
 * vectors.
 */
void VgeCamera::setViewDirectionMatrix(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
{
    const glm::vec3 w{ glm::normalize(direction) };         // along z-axis
    const glm::vec3 u{ glm::normalize(glm::cross(w, up)) }; // along x-axis
    const glm::vec3 v{ glm::cross(w, u) };                  // cross wu is normalized to y-axis

    m_viewMatrix = glm::mat4{ 1.f }; // set diagnal to identity matrix
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

    // Orthogonal matrices: inverse = transpose. Swap rows/cols of above.
    m_inverseViewMatrix = glm::mat4{ 1.f };
    m_inverseViewMatrix[0][0] = u.x;
    m_inverseViewMatrix[0][1] = u.y;
    m_inverseViewMatrix[0][2] = u.z;
    m_inverseViewMatrix[1][0] = v.x;
    m_inverseViewMatrix[1][1] = v.y;
    m_inverseViewMatrix[1][2] = v.z;
    m_inverseViewMatrix[2][0] = w.x;
    m_inverseViewMatrix[2][1] = w.y;
    m_inverseViewMatrix[2][2] = w.z;
    m_inverseViewMatrix[3][0] = position.x;
    m_inverseViewMatrix[3][1] = position.y;
    m_inverseViewMatrix[3][2] = position.z;
}

/* Sets the camera's view direction to point at a target.
 *
 * Adjusts the view direction so the camera is oriented towards a target from
 * a given position, using the provided up vector for orientation.
 */
// TODO: Add assertion making sure the direction is non-zero
void VgeCamera::setViewTargetDirectionMatrix(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
    setViewDirectionMatrix(position, target - position, up);
}

/* Sets the camera's View Matrix using Tait-Bryan angles (Y1X2Z3).
 *
 * Constructs a view matrix based on the camera's position and its rotation
 * defined by yaw (Y1), pitch (X2), and roll (Z3) angles. The rotation matrix is
 * derived from these angles, and the view matrix is updated accordingly.
 */
void VgeCamera::setViewYXZMatrix(glm::vec3 position, glm::vec3 rotation)
{
    // s and c represents sine and cosine
    const float s1 = glm::sin(rotation.y);
    const float s2 = glm::sin(rotation.x);
    const float s3 = glm::sin(rotation.z);
    const float c1 = glm::cos(rotation.y);
    const float c2 = glm::cos(rotation.x);
    const float c3 = glm::cos(rotation.z);

    // Tait-Bryan angles using Y1X2Z3
    const glm::vec3 u{
        (c1 * c3 + s1 * s2 * s3),
        (c2 * s3),
        (c1 * s2 * s3 - c3 * s1),
    };
    const glm::vec3 v{
        (c3 * s1 * s2 - c1 * s3),
        (c2 * c3),
        (c1 * c3 * s2 + s1 * s3),
    };
    const glm::vec3 w{
        (c2 * s1),
        (-s2),
        (c1 * c2),
    };

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

    // Orthogonal matrices: inverse = transpose. Swap rows/cols of above.
    m_inverseViewMatrix = glm::mat4{ 1.f };
    m_inverseViewMatrix[0][0] = u.x;
    m_inverseViewMatrix[0][1] = u.y;
    m_inverseViewMatrix[0][2] = u.z;
    m_inverseViewMatrix[1][0] = v.x;
    m_inverseViewMatrix[1][1] = v.y;
    m_inverseViewMatrix[1][2] = v.z;
    m_inverseViewMatrix[2][0] = w.x;
    m_inverseViewMatrix[2][1] = w.y;
    m_inverseViewMatrix[2][2] = w.z;
    m_inverseViewMatrix[3][0] = position.x;
    m_inverseViewMatrix[3][1] = position.y;
    m_inverseViewMatrix[3][2] = position.z;
}

// Returns the current projection matrix.
const glm::mat4& VgeCamera::getProjectionMatrix() const
{
    return m_projectionMatrix;
}

// Returns the current view matrix.
const glm::mat4& VgeCamera::getViewMatrix() const
{
    return m_viewMatrix;
}

// Returns the current inverse view matrix.
const glm::mat4& VgeCamera::getInverseViewMatrix() const
{
    return m_inverseViewMatrix;
}
} // namespace vge
