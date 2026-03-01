#include "Core/Graphics/Vulkan/vkPipelineLayout.h"
#include "Core/Graphics/Vulkan/vkBackend.h"

Hubris::Graphics::Vulkan::VulkanPipelineLayout::VulkanPipelineLayout(const std::vector<DescriptorSetLayout> &setLayouts, const std::vector<PushConstantRange> &pushConstants)
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;            // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(VulkanBackend::GetDevice(), &pipelineLayoutInfo, nullptr, &this->m_pipelineLayout) != VK_SUCCESS)
    {
        // throw std::runtime_error("failed to create pipeline layout!");
        Logger::Fatal("failed to create pipeline layout");
        return;
    }
}