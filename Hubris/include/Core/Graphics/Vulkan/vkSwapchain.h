#pragma once
#include "../Swapchain.h"
#include "volk.h"

namespace Hubris::Graphics::Vulkan {

	class vkSwapchain : public Hubris::Graphics::Swapchain {
	private:
		VkSwapchainKHR handle = VK_NULL_HANDLE;
		VkExtent2D swapChainExtent = VkExtent2D(0, 0);
		VkFormat swapChainImageFormat = VK_FORMAT_UNDEFINED;
		uint32_t imageCount = 0;
		std::vector<VkImage> images;
		std::vector<VkImageView> swapChainImageViews;
		SwapchainResult LastOperation = SwapchainResult::Success;
	public:
		vkSwapchain(VkSwapchainKHR swapchain, VkFormat format, VkExtent2D extent) noexcept;
		~vkSwapchain() noexcept;
		// Inherited via Swapchain
		SwapchainResult AcquireNextImage(uint32_t& imageIndex) override;
		void* GetImage(uint32_t imageIndex) const override;
		SwapchainResult Present(uint32_t imageIndex) override;
		void Resize(uint32_t width, uint32_t height) override;
		size_t GetImageCount() const override;
		virtual Format GetImageFormat() const noexcept override;
		virtual bool IsValid() const noexcept override;
		virtual void Destroy() noexcept;
	};
}