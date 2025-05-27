#pragma once
#include <cstdint>

namespace Sphynx::Graphics {

    enum class SwapchainResult {
        Success,
        Timeout,
        Suboptimal,
        OutOfDate,
        Error
    };

    class Swapchain {
    public:
        virtual ~Swapchain() = default;

        // Prepares or acquires the next image to render into
        virtual SwapchainResult AcquireNextImage(uint32_t& imageIndex) = 0;

        // Returns an abstract image handle or pointer to image/framebuffer
        virtual void* GetImage(uint32_t imageIndex) const = 0;

        // Presents the rendered image
        virtual SwapchainResult Present(uint32_t imageIndex) = 0;

        // Resize the swapchain
        virtual void Resize(uint32_t width, uint32_t height) = 0;

        // Number of images
        virtual size_t GetImageCount() const = 0;
    };
}
#pragma once
#include <cstdint>

namespace Sphynx::Graphics {

    enum class SwapchainResult {
        Success,
        Timeout,
        Suboptimal,
        OutOfDate,
        Error
    };

    class Swapchain {
    public:
        virtual ~Swapchain() = default;

        // Prepares or acquires the next image to render into
        virtual SwapchainResult AcquireNextImage(uint32_t& imageIndex) = 0;

        // Returns an abstract image handle or pointer to image/framebuffer
        virtual void* GetImage(uint32_t imageIndex) const = 0;

        // Presents the rendered image
        virtual SwapchainResult Present(uint32_t imageIndex) = 0;

        // Resize the swapchain
        virtual void Resize(uint32_t width, uint32_t height) = 0;

        // Number of images
        virtual size_t GetImageCount() const = 0;
    };
}
