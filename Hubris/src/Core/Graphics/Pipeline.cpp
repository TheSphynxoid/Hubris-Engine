#include "pch.h"
#include "Core/Graphics/Pipeline.h"
#ifdef HBR_WINDOWS
#include "Core/Graphics/Vulkan/vkPipeline.h"
#endif

Hubris::Handle<Hubris::Graphics::Pipeline> Hubris::Graphics::Pipeline::Create(const Hubris::Graphics::PipelineDescriptor& desc)
{
    if constexpr (Platform == Hbr_Platform::Windows){
        return new Hubris::Graphics::Vulkan::VulkanPipeline(desc);
    }
    assert("Platform unsupported or unknown");
    return nullptr;
}