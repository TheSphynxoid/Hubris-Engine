#pragma once
#include "Core/Graphics/Pipeline.h"
#include "volk.h"

namespace Hubris::Graphics::Vulkan
{
    class VulkanPipeline final : public Pipeline{
    private:
        VkPipeline graphicsPipeline = nullptr;
        // Holds a lazily-created empty layout when the PipelineDescriptor omits
        // one (constructive default lives in the factory, not in the struct).
        // When the caller supplies a layout, this stays empty and the caller's
        // Handle owns it.
        Handle<PipelineLayout> m_fallbackLayout;
    public:
        VulkanPipeline(const PipelineDescriptor& desc);
        ~VulkanPipeline();

    };
} // namespace Hubris::Graphics

