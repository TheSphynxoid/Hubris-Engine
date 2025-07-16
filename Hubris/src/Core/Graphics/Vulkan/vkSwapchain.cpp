#include "pch.h"
#include "Core/Graphics/Vulkan/vkSwapchain.h"
#include "Core/Graphics/Vulkan/Utility.h"
#include "Core/Graphics/Vulkan/vkBackend.h"

using namespace Hubris::Graphics;
using namespace Hubris::Graphics::Vulkan;

Hubris::Graphics::Vulkan::vkSwapchain::vkSwapchain(VkSwapchainKHR swapchain, VkFormat format, VkExtent2D extent) noexcept : handle(swapchain),
    swapChainExtent(extent), swapChainImageFormat(format)
{
    if (swapchain == VK_NULL_HANDLE) {
        Logger::Log("VulkanSwapchain: default-constructed into invalid state.");
        return;
    }
    vkGetSwapchainImagesKHR(vkBackend::GetDevice(), swapchain, &imageCount, nullptr);
    images.resize(imageCount);
    swapChainImageViews.resize(imageCount);
    vkGetSwapchainImagesKHR(vkBackend::GetDevice(), swapchain, &imageCount, images.data());



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
        if (vkCreateImageView(vkBackend::GetDevice(), &createInfo, vkBackend::GetAllocator(), &swapChainImageViews[i]) != VK_SUCCESS) {
            Logger::Fatal("Failed to create image view.");
            return;
        }
    }

}

Hubris::Graphics::Vulkan::vkSwapchain::~vkSwapchain()
{
    Destroy();
}


SwapchainResult Hubris::Graphics::Vulkan::vkSwapchain::AcquireNextImage(uint32_t& imageIndex)
{
    return SwapchainResult();
}

void* Hubris::Graphics::Vulkan::vkSwapchain::GetImage(uint32_t imageIndex) const
{
    return nullptr;
}

SwapchainResult Hubris::Graphics::Vulkan::vkSwapchain::Present(uint32_t imageIndex)
{
    return SwapchainResult();
}

void Hubris::Graphics::Vulkan::vkSwapchain::Resize(uint32_t width, uint32_t height)
{
}

size_t Hubris::Graphics::Vulkan::vkSwapchain::GetImageCount() const
{
    return imageCount;
}

Format Hubris::Graphics::Vulkan::vkSwapchain::GetImageFormat() const noexcept
{
    return VkFormatToFormat(swapChainImageFormat);
}

bool Hubris::Graphics::Vulkan::vkSwapchain::IsValid() const noexcept
{
    return handle != VK_NULL_HANDLE;
}

void Hubris::Graphics::Vulkan::vkSwapchain::Destroy() noexcept
{
    vkDestroySwapchainKHR(vkBackend::GetDevice(), handle, vkBackend::GetAllocator());
    handle = VK_NULL_HANDLE;
    swapChainExtent.height = 0;
    swapChainExtent.width = 0;
    swapChainImageFormat = VK_FORMAT_UNDEFINED;
    for (auto imageview : swapChainImageViews) {
         
        vkDestroyImageView(vkBackend::GetDevice(), imageview, vkBackend::GetAllocator());
    }
}
