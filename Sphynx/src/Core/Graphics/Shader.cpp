#include "pch.h"
#include "Core/Graphics/Shader.h"
#include "Core/Graphics/Vulkan/vkShader.h"

using namespace Sphynx;

Handle<Graphics::Shader> Graphics::Shader::Create(const std::vector<char>& data, ShaderStage type)
{
    if constexpr (Platform == Sph_Platform::Windows) {
        return new Vulkan::vkShader(data, type);
    }
}
