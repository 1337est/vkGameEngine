#pragma once

#include <vector>

namespace vge {
class VgeValidationLayers {
public:
    VgeValidationLayers();

    bool areVLayersEnabled() const;
    bool areVLayersSupported() const;
    const std::vector<const char*>& getVLayers() const;

private:
    void checkVLayerSupport();

    const bool m_enableVLayers;
    const std::vector<const char*> m_VLayers; // = { "VK_LAYER_KHRONOS_validation" };
    bool m_VLayerSupport;
};
} // namespace vge
