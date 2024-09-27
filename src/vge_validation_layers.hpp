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

    bool areValidationLayersEnabled() const
    {
        return m_enableValidationLayers;
    }

    const std::vector<const char*>& getValidationLayers() const
    {
        return m_validationLayers;
    }

    bool checkValidationLayerSupport() const;

private:
    const bool m_enableValidationLayers;
    const std::vector<const char*> m_validationLayers;
};
} // namespace vge
