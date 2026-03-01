#include "pch.h"
#include "Core/Graphics/Vulkan/vkSwapchain.h"
#include "Core/Graphics/Vulkan/Utility.h"
#include "Core/Graphics/Vulkan/vkBackend.h"

using namespace Hubris::Graphics;
using namespace Hubris::Graphics::Vulkan;

Hubris::Graphics::Vulkan::VulkanSwapchain::VulkanSwapchain(VkSwapchainKHR swapchain, VkFormat format, VkExtent2D extent) noexcept : handle(swapchain),
    swapChainExtent(extent), swapChainImageFormat(format)
{
    if (swapchain == VK_NULL_HANDLE) {
        Logger::Log("VulkanSwapchain: default-constructed into an incomplete state.");
        return;
    }
    vkGetSwapchainImagesKHR(VulkanBackend::GetDevice(), swapchain, &imageCount, nullptr);
    images.resize(imageCount);
    swapChainImageViews.resize(imageCount);
    vkGetSwapchainImagesKHR(VulkanBackend::GetDevice(), swapchain, &imageCount, images.data());



    for (unsigned int i = 0; i < imageCount; i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(VulkanBackend::GetDevice(), &createInfo, VulkanBackend::GetAllocator(), &swapChainImageViews[i]) != VK_SUCCESS) {
            Logger::Fatal("Failed to create image view.");
            return;
        }
    }

}

Hubris::Graphics::Vulkan::VulkanSwapchain::~VulkanSwapchain()
{
    Destroy();
}


SwapchainResult Hubris::Graphics::Vulkan::VulkanSwapchain::AcquireNextImage(uint32_t& imageIndex)
{
    return SwapchainResult();
}

void* Hubris::Graphics::Vulkan::VulkanSwapchain::GetImage(uint32_t imageIndex) const
{
    return nullptr;
}

SwapchainResult Hubris::Graphics::Vulkan::VulkanSwapchain::Present(uint32_t imageIndex)
{
    return SwapchainResult();
}

void Hubris::Graphics::Vulkan::VulkanSwapchain::Resize(uint32_t width, uint32_t height)
{
}

size_t Hubris::Graphics::Vulkan::VulkanSwapchain::GetImageCount() const
{
    return imageCount;
}

Format Hubris::Graphics::Vulkan::VulkanSwapchain::GetImageFormat() const noexcept
{
    return VkFormatToFormat(swapChainImageFormat);
}

bool Hubris::Graphics::Vulkan::VulkanSwapchain::IsValid() const noexcept
{
    return handle != VK_NULL_HANDLE;
}

void Hubris::Graphics::Vulkan::VulkanSwapchain::Destroy() noexcept
{
    vkDestroySwapchainKHR(VulkanBackend::GetDevice(), handle, VulkanBackend::GetAllocator());
    handle = VK_NULL_HANDLE;
    swapChainExtent.height = 0;
    swapChainExtent.width = 0;
    swapChainImageFormat = VK_FORMAT_UNDEFINED;
    for (auto imageview : swapChainImageViews) {
         
        vkDestroyImageView(VulkanBackend::GetDevice(), imageview, VulkanBackend::GetAllocator());
    }
}
