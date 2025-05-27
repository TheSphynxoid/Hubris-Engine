#pragma once
#include "../Renderer.h"
#include "volk.h"

namespace Sphynx::Graphics::Vulkan{
    struct Queue {
        VkQueue queue;
        uint32_t index;
    };
    class vkRenderer final : public Renderer {
    private:
        static inline Queue GraphicsQueue;
        static inline Queue PresentQueue;

        friend class vkBackend;
    public:
        inline static VkQueue GetGraphicsQueue() noexcept { return GraphicsQueue.queue; }
        inline static uint32_t GetGraphicsQueueIndex() noexcept { return GraphicsQueue.index; }
        inline static VkQueue GetPresentQueue() noexcept { return PresentQueue.queue; };
        inline static uint32_t GetPresentQueueIndex() noexcept { return PresentQueue.index; }

    };
}
