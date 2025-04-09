#pragma once
#include "../Renderer.h"
#include "volk.h"

namespace Sphynx::Graphics::Vulkan{
    class vkRenderer final : public Renderer {
    private:
        static inline VkQueue GraphicsQueue;
        static inline VkQueue PresentQueue;
        static inline VkSwapchainKHR SwapChain = nullptr;
    public:
    };
}
