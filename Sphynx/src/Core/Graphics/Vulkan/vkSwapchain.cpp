#include "pch.h"
#include "Core/Graphics/Vulkan/vkSwapchain.h"
#include "Core/Graphics/Vulkan/Utility.h"

using namespace Sphynx::Graphics;
using namespace Sphynx::Graphics::Vulkan;

Sphynx::Graphics::Vulkan::vkSwapchain::vkSwapchain(VkSwapchainKHR swapchain, VkFormat format, VkExtent2D extent) : handle(swapchain),
    swapChainExtent(extent), swapChainImageFormat(format)
{
    if (swapchain == VK_NULL_HANDLE) {
        Logger::Log("VulkanSwapchain: default-constructed into invalid state.");
        return;
    }
    vkGetSwapchainImagesKHR(vkBackend::GetDevice(), swapchain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(vkBackend::GetDevice(), swapchain, &imageCount, images.data());

}

Sphynx::Graphics::Vulkan::vkSwapchain::~vkSwapchain()
{
    Destroy();
}


SwapchainResult Sphynx::Graphics::Vulkan::vkSwapchain::AcquireNextImage(uint32_t& imageIndex)
{
    return SwapchainResult();
}

void* Sphynx::Graphics::Vulkan::vkSwapchain::GetImage(uint32_t imageIndex) const
{
    return nullptr;
}

SwapchainResult Sphynx::Graphics::Vulkan::vkSwapchain::Present(uint32_t imageIndex)
{
    return SwapchainResult();
}

void Sphynx::Graphics::Vulkan::vkSwapchain::Resize(uint32_t width, uint32_t height)
{
}

size_t Sphynx::Graphics::Vulkan::vkSwapchain::GetImageCount() const
{
    return imageCount;
}

Format Sphynx::Graphics::Vulkan::vkSwapchain::GetImageFormat() const noexcept
{
    return VkFormatToFormat(swapChainImageFormat);
}

bool Sphynx::Graphics::Vulkan::vkSwapchain::IsValid() const noexcept
{
    return handle != VK_NULL_HANDLE;
}

void Sphynx::Graphics::Vulkan::vkSwapchain::Destroy() noexcept
{
    vkDestroySwapchainKHR(vkBackend::GetDevice(), handle, vkBackend::GetAllocator());
    handle = VK_NULL_HANDLE;
    swapChainExtent.height = 0;
    swapChainExtent.width = 0;
    swapChainImageFormat = VK_FORMAT_UNDEFINED;
}
