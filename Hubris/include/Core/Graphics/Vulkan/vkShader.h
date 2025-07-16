#pragma once
#include <Core/Graphics/Shader.h>
#include <Core/Graphics/Vulkan/vkBackend.h>

namespace Hubris::Graphics::Vulkan {
	class vkShader : public Shader {
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
	};
}