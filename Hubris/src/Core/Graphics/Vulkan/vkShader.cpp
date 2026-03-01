#include "pch.h"
#include "Core/Graphics/Vulkan/vkShader.h"

using namespace Hubris::Graphics;

Hubris::Graphics::Vulkan::VulkanShader::VulkanShader(const std::span<const uint8_t>& code, ShaderStage _stage,
	std::string _entryPoint) : Shader(_stage), entryPoint(_entryPoint) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
	

	if (vkCreateShaderModule(VulkanBackend::GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		Logger::Log("failed to create shader module!");
		stage = ShaderStage::Unknown;
		return;
	}
	Logger::Log("{} Shader created succesfully.", VertexStageToString(_stage));

}
