#pragma once
#include "Core/Graphics/Enums.h"

namespace Hubris::Graphics
{
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

        static Handle<Image> Create(uint32_t width, uint32_t height,
            Format format, uint32_t mipLevels = 1, ImageMemoryType memoryType = ImageMemoryType::GPU_LOCAL);

    protected:
        Image() = default;
    };

} // namespace Hubris::Graphics
