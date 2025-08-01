#pragma once
#include "Core/Graphics/Format.h"

namespace Hubris::Graphics
{
    enum class ImageLayout {
        UNDEFINED,
        COLOR_ATTACHMENT,
        DEPTH_STENCIL_ATTACHMENT,
        SHADER_READ_ONLY,
        TRANSFER_SRC,
        TRANSFER_DST
    };

    enum class ImageMemoryType {
        GPU_LOCAL,
        CPU_VISIBLE,
        CPU_TO_GPU
    };

    enum class Swizzle : unsigned char {
        X, Y, Z, W
    };

    class Image {
    public:
        virtual ~Image() = default;

        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual void Unbind() const = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual Format GetFormat() const = 0;

        virtual void SetData(const void* data, uint32_t size) = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;

        static std::shared_ptr<Image> Create(uint32_t width, uint32_t height,
            Format format, uint32_t mipLevels = 1, ImageMemoryType memoryType = ImageMemoryType::GPU_LOCAL);

    protected:
        Image() = default;
    };

} // namespace Hubris::Graphics
