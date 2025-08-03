#include "pch.h"
#include "Core/Graphics/Pipeline.h"
#ifdef HBR_WINDOWS
#include "Core/Graphics/Vulkan/vkPipeline.h"
#endif

Hubris::Handle<Hubris::Graphics::Pipeline> Hubris::Graphics::Pipeline::Create(const Hubris::Graphics::PipelineDescriptor& shaders)
{
    if constexpr (Platform == Hbr_Platform::Windows){
        return new Hubris::Graphics::vkPipeline(shaders);
    }
    assert("Platform unsupported or unknown");
    return nullptr;
}