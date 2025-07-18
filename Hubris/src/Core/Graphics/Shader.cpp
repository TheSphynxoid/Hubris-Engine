#include "pch.h"
#include "Core/Graphics/Shader.h"
#include "Core/Graphics/Vulkan/vkShader.h"

using namespace Hubris;

Handle<Graphics::Shader> Graphics::Shader::Create(const std::vector<char>& data, ShaderStage type)
{
    if constexpr (Platform == Hbr_Platform::Windows) {
        return new Vulkan::vkShader(data, type);
    }
}
