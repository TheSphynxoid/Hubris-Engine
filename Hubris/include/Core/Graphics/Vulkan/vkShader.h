#pragma once
#include <Core/Graphics/Shader.h>
#include <Core/Graphics/Vulkan/Utility.h>
#include <Core/Graphics/Vulkan/vkBackend.h>

namespace Hubris::Graphics::Vulkan {
	class VulkanShader final : public Shader {
	private:
		VkShaderModule shaderModule;
		std::string entryPoint = "main";
	public:
		VulkanShader(const std::span<const uint8_t>& code, ShaderStage stage, std::string entryPoint = "main");
		~VulkanShader() noexcept {
			Destroy();
		}

		virtual void Destroy() noexcept {
			vkDestroyShaderModule(VulkanBackend::GetDevice(), shaderModule, VulkanBackend::GetAllocator());
			stage = ShaderStage::Unknown;
		}

		VkPipelineShaderStageCreateInfo GetShaderPipelineCreateInfo()const noexcept {
			VkPipelineShaderStageCreateInfo ShaderStageInfo{};
			ShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			ShaderStageInfo.stage = VkShaderStageToFlags(stage);
			ShaderStageInfo.module = shaderModule;
			ShaderStageInfo.pName = "main";
			return ShaderStageInfo;
		}
	};
}