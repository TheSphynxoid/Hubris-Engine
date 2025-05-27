#pragma once

#include <cstdint>
#include <cstddef>

#define DEFINE_FORMAT(format, name, componentType, size, channels, isdepth, isSRGB) \
{format, name, componentType, size, channels, isdepth, isSRGB }

namespace Sphynx::Graphics {

    // Enum representing component data types.
    enum class ComponentType {
        Unknown,
        UNorm,
        SNorm,
        UInt = 1 << 2,
        SInt = 1 << 3,
        Float = 1 << 4,
        Depth = 1 << 5,
        Stencil = 1 << 6
    };

    // Enum representing pixel/texture formats.
    enum class Format {
        Undefined,

        R8Unorm,
        R8Snorm,
        R8Uint,
        R8Sint,

        R8G8Unorm,
        R8G8Snorm,
        R8G8Uint,
        R8G8Sint,

        R8G8B8A8Unorm,
        R8G8B8A8Snorm,
        R8G8B8A8Uint,
        R8G8B8A8Sint,
        R8G8B8A8Srgb,

        B8G8R8A8Unorm,
        B8G8R8A8Srgb,

        A2R10G10B10Unorm,

        R16Float,
        R16Uint,
        R16Sint,

        R16G16Float,
        R16G16Uint,
        R16G16Sint,

        R16G16B16A16Float,
        R16G16B16A16Uint,
        R16G16B16A16Sint,

        R32Uint,
        R32Sint,
        R32Float,

        R32G32Uint,
        R32G32Sint,
        R32G32Float,

        R32G32B32A32Uint,
        R32G32B32A32Sint,
        R32G32B32A32Float,

        D16Unorm,
        D24UnormS8Uint,
        D32Sfloat,
        D32SfloatS8Uint,

        Count
    };

    // Struct holding all traits of a format.
    struct FormatInfo {
        Format format;
        const char* name;
        ComponentType type;
        uint32_t size;
        uint32_t channels;
        bool isDepth;
        bool isSrgb;
    };

    // Central data table. TODO: Simplify declarations using the DEFINE_FORMAT macro.
    constexpr FormatInfo formatTable[] = {
        { Format::Undefined,              "Undefined",             ComponentType::Unknown,     0, 0, false, false },

        { Format::R8Unorm,                "R8Unorm",               ComponentType::UNorm,       1, 1, false, false },
        { Format::R8Snorm,                "R8Snorm",               ComponentType::SNorm,       1, 1, false, false },
        { Format::R8Uint,                 "R8Uint",                ComponentType::UInt,        1, 1, false, false },
        { Format::R8Sint,                 "R8Sint",                ComponentType::SInt,        1, 1, false, false },

        { Format::R8G8Unorm,              "R8G8Unorm",             ComponentType::UNorm,       2, 2, false, false },
        { Format::R8G8Snorm,              "R8G8Snorm",             ComponentType::SNorm,       2, 2, false, false },
        { Format::R8G8Uint,               "R8G8Uint",              ComponentType::UInt,        2, 2, false, false },
        { Format::R8G8Sint,               "R8G8Sint",              ComponentType::SInt,        2, 2, false, false },

        { Format::R8G8B8A8Unorm,          "R8G8B8A8Unorm",         ComponentType::UNorm,       4, 4, false, false },
        { Format::R8G8B8A8Snorm,          "R8G8B8A8Snorm",         ComponentType::SNorm,       4, 4, false, false },
        { Format::R8G8B8A8Uint,           "R8G8B8A8Uint",          ComponentType::UInt,        4, 4, false, false },
        { Format::R8G8B8A8Sint,           "R8G8B8A8Sint",          ComponentType::SInt,        4, 4, false, false },
        { Format::R8G8B8A8Srgb,           "R8G8B8A8Srgb",          ComponentType::UNorm,       4, 4, false, true  },

        { Format::B8G8R8A8Unorm,          "B8G8R8A8Unorm",         ComponentType::UNorm,       4, 4, false, false },
        { Format::B8G8R8A8Srgb,           "B8G8R8A8Srgb",          ComponentType::UNorm,       4, 4, false, true  },

        { Format::A2R10G10B10Unorm,       "A2R10G10B10Unorm",      ComponentType::UNorm,       4, 4, false, false },

        { Format::R16Float,               "R16Float",              ComponentType::Float,       2, 1, false, false },
        { Format::R16Uint,                "R16Uint",               ComponentType::UInt,        2, 1, false, false },
        { Format::R16Sint,                "R16Sint",               ComponentType::SInt,        2, 1, false, false },

        { Format::R16G16Float,            "R16G16Float",           ComponentType::Float,       4, 2, false, false },
        { Format::R16G16Uint,             "R16G16Uint",            ComponentType::UInt,        4, 2, false, false },
        { Format::R16G16Sint,             "R16G16Sint",            ComponentType::SInt,        4, 2, false, false },

        { Format::R16G16B16A16Float,      "R16G16B16A16Float",     ComponentType::Float,       8, 4, false, false },
        { Format::R16G16B16A16Uint,       "R16G16B16A16Uint",      ComponentType::UInt,        8, 4, false, false },
        { Format::R16G16B16A16Sint,       "R16G16B16A16Sint",      ComponentType::SInt,        8, 4, false, false },

        { Format::R32Uint,                "R32Uint",               ComponentType::UInt,        4, 1, false, false },
        { Format::R32Sint,                "R32Sint",               ComponentType::SInt,        4, 1, false, false },
        { Format::R32Float,               "R32Float",              ComponentType::Float,       4, 1, false, false },

        { Format::R32G32Uint,             "R32G32Uint",            ComponentType::UInt,        8, 2, false, false },
        { Format::R32G32Sint,             "R32G32Sint",            ComponentType::SInt,        8, 2, false, false },
        { Format::R32G32Float,            "R32G32Float",           ComponentType::Float,       8, 2, false, false },

        { Format::R32G32B32A32Uint,       "R32G32B32A32Uint",      ComponentType::UInt,       16, 4, false, false },
        { Format::R32G32B32A32Sint,       "R32G32B32A32Sint",      ComponentType::SInt,       16, 4, false, false },
        { Format::R32G32B32A32Float,      "R32G32B32A32Float",     ComponentType::Float,      16, 4, false, false },

        { Format::D16Unorm,               "D16Unorm",              ComponentType::Depth,2, 1, true,  false },
        { Format::D24UnormS8Uint,         "D24UnormS8Uint",        ComponentType::Depth,4, 2, true,  false },
        { Format::D32Sfloat,              "D32Sfloat",             ComponentType::Depth,4, 1, true,  false },
        { Format::D32SfloatS8Uint,        "D32SfloatS8Uint",       ComponentType::Depth,5, 2, true,  false }
    };

    constexpr const FormatInfo* getFormatInfo(Format fmt) {
        for (const auto& f : formatTable)
            if (f.format == fmt)
                return &f;
        return nullptr;
    }

    constexpr const char* formatName(Format fmt) {
        auto* info = getFormatInfo(fmt);
        return info ? info->name : "Unknown";
    }

    constexpr ComponentType componentType(Format fmt) {
        auto* info = getFormatInfo(fmt);
        return info ? info->type : ComponentType::Unknown;
    }

    constexpr size_t formatSize(Format fmt) {
        auto* info = getFormatInfo(fmt);
        return info ? info->size : 0;
    }

    constexpr uint32_t formatChannels(Format fmt) {
        auto* info = getFormatInfo(fmt);
        return info ? info->channels : 0;
    }

    constexpr bool isDepthFormat(Format fmt) {
        auto* info = getFormatInfo(fmt);
        return info ? info->isDepth : false;
    }

    constexpr bool isSrgbFormat(Format fmt) {
        auto* info = getFormatInfo(fmt);
        return info ? info->isSrgb : false;
    }

    // Traits wrapper.
    template<Format Fmt>
    struct FormatTraits {
        static constexpr bool IsColor = !isDepthFormat(Fmt);
        static constexpr size_t Size = formatSize(Fmt);
        static constexpr ComponentType Type = componentType(Fmt);
        static constexpr uint32_t Channels = formatChannels(Fmt);
        static constexpr bool IsSrgb = isSrgbFormat(Fmt);
    };

} // namespace Sphynx
