#pragma once

// std
#include <vector>

namespace vge
{

class VgeValidationLayers
{
public:
    VgeValidationLayers();
    ~VgeValidationLayers();

    bool enableValidationLayers() const
    {
        return m_enableValidationLayers;
    }

    bool checkValidationLayerSupport() const;

private:
    const bool m_enableValidationLayers;
    const std::vector<const char*> m_validationLayers;
};
} // namespace vge
