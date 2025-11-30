#pragma once
#include <Core/Graphics/Shader.h>
#include <Core/Graphics/Vulkan/Utility.h>
#include <Core/Graphics/Vulkan/vkBackend.h>

namespace Hubris::Graphics::Vulkan {
	class vkShader final : public Shader {
	private:
		VkShaderModule shaderModule;
		ShaderStage stage = ShaderStage::Unknown;
	public:
		vkShader(const std::vector<char>& code, ShaderStage stage);
		~vkShader() noexcept {
			Destroy();
		}

		virtual void Destroy() noexcept {
			vkDestroyShaderModule(vkBackend::GetDevice(), shaderModule, vkBackend::GetAllocator());
			stage = ShaderStage::Unknown;
		}

		VkPipelineShaderStageCreateInfo GetShaderPipelineCreateInfo()noexcept {
			VkPipelineShaderStageCreateInfo ShaderStageInfo{};
			ShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			ShaderStageInfo.stage = VkShaderStageToFlags(stage);
			ShaderStageInfo.module = shaderModule;
			ShaderStageInfo.pName = "main";
			return ShaderStageInfo;
		}
	};
}