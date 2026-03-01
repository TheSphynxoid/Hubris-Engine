#include "Core/Graphics/PipelineLayout.h"
#include "Core/Graphics/Vulkan/vkPipelineLayout.h"

using namespace Hubris;
using namespace Hubris::Graphics;

Handle<PipelineLayout> PipelineLayout::Create(const std::vector<DescriptorSetLayout> &setLayouts, const std::vector<PushConstantRange> &pushConstants)
{
    if constexpr (Platform == Hbr_Platform::Windows) {
        return new Vulkan::VulkanPipelineLayout(setLayouts, pushConstants);
    }
    return nullptr;
}