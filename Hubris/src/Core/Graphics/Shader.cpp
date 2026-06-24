#include "pch.h"
#include "Core/Graphics/Shader.h"
#include "Core/Graphics/Vulkan/vkShader.h"

using namespace Hubris;

Handle<Graphics::Shader> Graphics::Shader::Create(const std::span<const uint8_t>& data, ShaderStage type,
    std::string entryPoint)
{
    if constexpr (Platform == Hbr_Platform::Windows) {
        return new Vulkan::VulkanShader(data, type, entryPoint);
    }
    assert(false && "Shader::Create: platform unsupported or unknown");
    return nullptr;
}
