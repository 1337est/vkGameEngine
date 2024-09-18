```cpp
// headers
#include "vge_camera.hpp"

// std
#include <cassert>
#include <limits>

namespace vge
{

/** Sets the Vulkan Canonical View Volume from an Orthographic View Volume via
 * an Orthographic Projection Matrix.
 *
 * OrthographicProjectionMatrix = ScaleMatrix * TranslationMatrix
 *
 * You can think of the Orthographic View Volume as a space where things exist
 * as they are naturally and is defined by 6 bounding planes. The Orthographic
 * Projection is a generalization of the view volume at some location in space.
 *
 * The Vulkan Canonical View Volume is what is displayed to us in Vulkan's
 * 2*2*1 space: x from(-1, 1), y from(-1, 1), z from(0, 1).
 *
 * This function defines an orthographic projection matrix based on the given
 * left, right, top, bottom, near, and far clipping planes. An orthographic
 * projection is a means of representing a 3D object in 2D.
 *
 * An orthographic projection is a form of parallel projection, where all the
 * projection lines are orthogonal to the projection plane, resulting in every
 * plane of the scene appearing in affine transformation.
 *
 * An affine transformation is a geometric transformation that preserves lines
 * and parallelism, but not Euclidean distances and angles.
 *
 * Positive and Negative axis alignment:
 * x-axis: Negative left,      Postive right
 * y-axis: Negative up,        Postive down
 * z-axis: Negative backwards, Postive forwards
 *
 * @param `left`   The left vertical clipping plane.
 * @param `right`  The right vertical clipping plane.
 * @param `top`    The top horizontal clipping plane.
 * @param `bottom` The bottom horizontal clipping plane.
 * @param `near`   The near depth clipping plane.
 * @param `far`    The far depth clipping plane.
 */
// TODO: Change name to setOrthographicProjectionMatrix
void VgeCamera::setOrthographicProjectionMatrix(
    float left,
    float right,
    float top,
    float bottom,
    float near,
    float far)
{
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

/** Sets the Vulkan Canonical View Volume from both the Orthographic View Volume
 * and Perspective View Volume (a square frustum) via a perspective projection
 * matrix.
 *
 * PerspectiveProjectionMatrix = OrthographicProjectionMatrix *
 * PerspectiveMatrix
 *
 * The creation of the PerspectiveMatrix uses homogeneous coordinates where the
 * homogeneous vector[x,y,z,w] coresponds to the position vector[x/w,y/w,z/w].
 * This is why the matrix's form is a little different. For a more detailed
 * explanation of this, see "The Math behind (most) 3D games - Perspective
 * Projection" by Brendan Galea.
 *
 * This function defines a perspective projection matrix based on the vertical
 * field of view (fovy), aspect ratio of the window (w / h), near, and far
 * planes.
 *
 * A perspective projection is a representation from the perspective of a viewer
 * where objects that are nearer are larger, and objects that are farther are
 * smaller. This also means that objects with lines which are normally parallel,
 * now converge towards a vanishing point (think railrod tracks that appear as
 * if they intersect far away, but in reality don't).
 *
 * @param `fovy` The vertical field of view in radians.
 * @param `aspect` The aspect ratio of the viewport (width/height).
 * @param `near` The near depth clipping plane.
 * @param `far` The far depth clipping plane.
 */
// TODO: Change name to setPerspectiveProjectionMattrix
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

/** Sets the camera's View Matrix from a Rotation Matrix and Translation Matrix
 *
 * We need a way to capture the camera's view for when the camera rotates and
 * translates. We can do this by defining the camera's position difference from
 * the origin, direction, and up vector for orientation to create the View
 * Matrix. We then need this new direction in arbitrary space to somehow
 * translate into a new coordinate system so that we still know where everything
 * is in space.
 *
 * We do this by constructing an Orthonormal Basis using orthonormal basis
 * vectors u, v, and w. The direction we are facing is assumed to be pointing in
 * the new positive z-axis (w). The direction along the new positive x-axis (u)
 * (where positive is right and negative is left) can be given by the
 * right-hand-rule with up pointing down (a negative y). The final direction
 * along the new positive y-axis (v) can be given with the right-hand-rule from
 * the new positive z (w) and x (u) axis.
 *
 * @param `position` The camera's position in world coordinates.
 * @param `direction` The direction the camera is facing.
 * @param `up` The up vector used to orient the camera.
 */
// TODO: Change name to setViewDirectionMatrix
void VgeCamera::setViewDirection(
    glm::vec3 position,
    glm::vec3 direction,
    glm::vec3 up)
{
    const glm::vec3 w{ glm::normalize(direction) };         // along z-axis
    const glm::vec3 u{ glm::normalize(glm::cross(w, up)) }; // along x-axis
    const glm::vec3 v{ glm::cross(w, u) }; // cross wu is normalized to y-axis

    /* Uses basis vectors to construct a rotation matrix, combined with a
     * translation matrix from the camera's position back to the origin. Notice
     * that glm uses col major vs row major.
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

    // Pretty straightforward: The inverse of above. Swap rows and cols
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

/** Sets the camera's view matrix by targeting a specific point.
 *
 * Sets the view matrix by pointing the camera at a target point in world space
 * from a given position, using the up vector for orientation. This ensures the
 * camera is pointed a the target.
 *
 * @param `position` The camera's position in world coordinates.
 * @param `target` The target point the camera should focus on.
 * @param `up` The up vector used to orient the camera.
 */
// TODO: Add assertion making sure the direction is non-zero
void VgeCamera::setViewTarget(
    glm::vec3 position,
    glm::vec3 target,
    glm::vec3 up)
{
    setViewDirection(position, target - position, up);
}

/** Sets the camera's view matrix using yaw, pitch, and roll angles.
 *
 * Constructs a view matrix based on the camera's position and its orientation
 * defined by the yaw (Y), pitch (X), and roll (Z) rotations.
 *
 * @param `position` The camera's position in world coordinates.
 * @param `rotation` The camera's rotation as a vector of yaw (y), pitch (x),
 * and roll (z).
 */
void VgeCamera::setViewYXZ(glm::vec3 position, glm::vec3 rotation)
{
    const float cY = glm::cos(rotation.y); // cosine of Y rotation angle
    const float cX = glm::cos(rotation.x); // cosine of X rotation angle
    const float cZ = glm::cos(rotation.z); // cosine of Z rotation angle
    const float sY = glm::sin(rotation.y); // sine of Y rotation angle
    const float sX = glm::sin(rotation.x); // sine of X rotation angle
    const float sZ = glm::sin(rotation.z); // sine of Z rotation angle

    // Constructing the positive directions based off rotations
    const glm::vec3 u{
        (cY * cZ + sY * sX * sZ),
        (cX * sZ),
        (cY * sX * sZ - cZ * sY),
    };
    const glm::vec3 v{
        (cZ * sY * sX - cY * sZ),
        (cX * cZ),
        (cY * cZ * sX + sY * sZ),
    };
    const glm::vec3 w{
        (cX * sY),
        (-sX),
        (cY * cX),
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
} // namespace vge
```
