#pragma once
#include <cstdint>

namespace Sphynx::Graphics {

    enum class PresentMode : unsigned char {
        Immediate,
        Mailbox,
        Fifo,
        FifoRelaxed
    };

    enum class SwapchainResult : unsigned char {
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
        virtual Format GetImageFormat() const noexcept = 0;
        /*virtual glm::vec2 GetSize()const noexcept = 0;*/
        virtual bool IsValid() const noexcept = 0;

        virtual void Destroy() noexcept = 0;
    };
}
