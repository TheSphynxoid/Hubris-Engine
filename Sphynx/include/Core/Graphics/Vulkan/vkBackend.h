#pragma once
#include <volk.h>
#include <Logger.h>
#include <vulkan/vulkan.hpp>

namespace Sphynx::Graphics::Vulkan {
	class VulkanBackend {
	private:
	public:
		void Init() {
			if (volkInitialize() != VK_SUCCESS) {
				Logger::Log("Unable to initialize volk");
			}
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

			std::cout << extensionCount << " extensions supported\n";
			//TODO: add user Application Name.
			VkApplicationInfo appInfo{};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "Testing";
			appInfo.applicationVersion = VK_MAKE_API_VERSION(0 ,1, 0, 0);
			appInfo.pEngineName = "Sphynx Engine";
			appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_3;
				
			VkInstanceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;
		}
	};
}