#pragma once
#include "Core/Graphics/Pipeline.h"
#include "volk.h"

namespace Hubris::Graphics::Vulkan
{
    class VulkanPipeline final : public Pipeline{
    private:
        VkPipeline graphicsPipeline = nullptr;
    public:
        VulkanPipeline(const PipelineDescriptor& desc);
        ~VulkanPipeline();
        
    };
} // namespace Hubris::Graphics

