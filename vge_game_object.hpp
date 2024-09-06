#pragma once

// headers
#include "vge_model.hpp"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>

namespace vge
{
struct TransformComponent
{
    glm::vec3 translation{};          // position offset
    glm::vec3 scale{ 1.f, 1.f, 1.f }; // identity matrix
    glm::vec3 rotation{};

    // Matrix transform corresponds to Translate * Ry * Rx * Rz * Scale
    // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
    // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
    glm::mat4 mat4();
    glm::mat3 normalMatrix();
};

class VgeGameObject

{
public:
    using id_t = unsigned int;

    static VgeGameObject createGameObject()
    {
        static id_t currentId = 0;
        return VgeGameObject{ currentId++ };
    }

    VgeGameObject(const VgeGameObject&) = delete;
    VgeGameObject& operator=(const VgeGameObject&) = delete;
    VgeGameObject(VgeGameObject&&) = default;
    VgeGameObject& operator=(VgeGameObject&&) = default;

    id_t getId()
    {
        return m_id;
    }

    std::shared_ptr<VgeModel> m_model{};
    glm::vec3 m_color{};
    TransformComponent m_transform{};

private:
    VgeGameObject(id_t objId)
        : m_id{ objId }
    {
    }

    id_t m_id;
};
} // namespace vge
