#pragma once
#include "../PipelineLayout.h"
#include "volk.h"

namespace Hubris::Graphics::Vulkan
{
    class VulkanPipelineLayout : public PipelineLayout
    {
    private:
        VkPipelineLayout m_pipelineLayout;
    public:
        VulkanPipelineLayout(const std::vector<DescriptorSetLayout> &setLayouts,
            const std::vector<PushConstantRange> &pushConstants);

        virtual void* GetNative()const noexcept { return m_pipelineLayout; };
    };
} // namespace Hubris::Graphics::Vulkan
