#include "vge_game_object.hpp"

namespace vge {
/* Constructs a VgeGameObject with a unique identifier.
 *
 * This constructor initializes the VgeGameObject with the given id_t object
 * identifier, allowing for unique identification of the game object within the
 * scene.
 */
VgeGameObject::VgeGameObject(id_t objId)
    : m_id{ objId }
{}

/* Creates and returns a new instance of VgeGameObject.
 *
 * This static method generates a new VgeGameObject with a unique identifier,
 * incrementing the current ID for each new object created. This ensures that
 * each game object has a distinct ID, which is essential for object management.
 */
VgeGameObject VgeGameObject::createGameObject()
{
    static id_t currentId = 0;
    return VgeGameObject{ currentId++ };
}

/* Retrieves the unique identifier of the VgeGameObject.
 *
 * This method returns the ID associated with the VgeGameObject, which can be
 * used for identification and management purposes within the game engine.
 */
id_t VgeGameObject::getId()
{
    return m_id;
}

/* Computes the transformation matrix for the TransformComponent.
 *
 * This method constructs a 4x4 transformation matrix based on the scale,
 * rotation, and translation of the TransformComponent. It accounts for the
 * object's orientation in 3D space and applies scaling factors, making it
 * suitable for rendering.
 */
glm::mat4 TransformComponent::mat4()
{
    {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);

        return glm::mat4{
            {
             scale.x * (c1 * c3 + s1 * s2 * s3),
             scale.x * (c2 * s3),
             scale.x * (c1 * s2 * s3 - c3 * s1),
             0.0f, },
            {
             scale.y * (c3 * s1 * s2 - c1 * s3),
             scale.y * (c2 * c3),
             scale.y * (c1 * c3 * s2 + s1 * s3),
             0.0f, },
            {
             scale.z * (c2 * s1),
             scale.z * (-s2),
             scale.z * (c1 * c2),
             0.0f, },
            { translation.x, translation.y, translation.z, 1.0f }
        };
    }
}

/* Computes the normal matrix for the TransformComponent.
 *
 * This method generates a 3x3 matrix that is used to transform normals
 * correctly based on the current scale and rotation of the TransformComponent.
 * It ensures that lighting calculations are accurate in 3D rendering by
 * maintaining the correct orientation of surface normals.
 */
glm::mat3 TransformComponent::normalMatrix()
{
    {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);

        const glm::vec3 invScale = 1.0f / scale;

        return glm::mat3{
            {
             invScale.x * (c1 * c3 + s1 * s2 * s3),
             invScale.x * (c2 * s3),
             invScale.x * (c1 * s2 * s3 - c3 * s1),
             },
            {
             invScale.y * (c3 * s1 * s2 - c1 * s3),
             invScale.y * (c2 * c3),
             invScale.y * (c1 * c3 * s2 + s1 * s3),
             },
            {
             invScale.z * (c2 * s1),
             invScale.z * (-s2),
             invScale.z * (c1 * c2),
             },
        };
    }
}

/* Creates a VgeGameObject configured as a point light source.
 *
 * This method constructs a VgeGameObject that acts as a point light, setting
 * its intensity and radius. It initializes the point light component and scales
 * the object based on the provided radius, allowing for realistic lighting
 * effects within the scene.
 */
VgeGameObject VgeGameObject::makePointLight(float intensity, float radius, glm::vec3 color)
{
    VgeGameObject gameObj = VgeGameObject::createGameObject();
    gameObj.m_color = color;
    gameObj.m_transform.scale.x = radius;
    gameObj.m_pointLight = std::make_unique<PointLightComponent>();
    gameObj.m_pointLight->lightIntensity = intensity;

    return gameObj;
}

} // namespace vge
