#include "pch.h"
#include "Core/Graphics/Vulkan/vkShader.h"


Sphynx::Graphics::Vulkan::vkShader::vkShader(const std::vector<char>& code, ShaderStage _stage) : Shader(_stage)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	if (vkCreateShaderModule(vkBackend::GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		Logger::Log("failed to create shader module!");
		stage = ShaderStage::Unknown;
		return;
	}

	stage = _stage;
}
