#pragma once
#include "../Format.h"
#include "volk.h"

namespace Hubris::Graphics::Vulkan {
    // Conversion from Format to VkFormat
    constexpr VkFormat FormatToVkFormat(Format format) noexcept {
        switch (format) {
        case Format::Undefined:         return VK_FORMAT_UNDEFINED;
        case Format::R8Unorm:           return VK_FORMAT_R8_UNORM;
        case Format::R8Snorm:           return VK_FORMAT_R8_SNORM;
        case Format::R8Uint:            return VK_FORMAT_R8_UINT;
        case Format::R8Sint:            return VK_FORMAT_R8_SINT;
        case Format::R8G8Unorm:         return VK_FORMAT_R8G8_UNORM;
        case Format::R8G8Snorm:         return VK_FORMAT_R8G8_SNORM;
        case Format::R8G8Uint:          return VK_FORMAT_R8G8_UINT;
        case Format::R8G8Sint:          return VK_FORMAT_R8G8_SINT;
        case Format::R8G8B8A8Unorm:     return VK_FORMAT_R8G8B8A8_UNORM;
        case Format::R8G8B8A8Snorm:     return VK_FORMAT_R8G8B8A8_SNORM;
        case Format::R8G8B8A8Uint:      return VK_FORMAT_R8G8B8A8_UINT;
        case Format::R8G8B8A8Sint:      return VK_FORMAT_R8G8B8A8_SINT;
        case Format::R8G8B8A8Srgb:      return VK_FORMAT_R8G8B8A8_SRGB;
        case Format::B8G8R8A8Unorm:     return VK_FORMAT_B8G8R8A8_UNORM;
        case Format::B8G8R8A8Srgb:      return VK_FORMAT_B8G8R8A8_SRGB;
        case Format::A2R10G10B10Unorm:  return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
        case Format::R16Float:          return VK_FORMAT_R16_SFLOAT;
        case Format::R16Uint:           return VK_FORMAT_R16_UINT;
        case Format::R16Sint:           return VK_FORMAT_R16_SINT;
        case Format::R16G16Float:       return VK_FORMAT_R16G16_SFLOAT;
        case Format::R16G16Uint:        return VK_FORMAT_R16G16_UINT;
        case Format::R16G16Sint:        return VK_FORMAT_R16G16_SINT;
        case Format::R16G16B16A16Float: return VK_FORMAT_R16G16B16A16_SFLOAT;
        case Format::R16G16B16A16Uint:  return VK_FORMAT_R16G16B16A16_UINT;
        case Format::R16G16B16A16Sint:  return VK_FORMAT_R16G16B16A16_SINT;
        case Format::R32Uint:           return VK_FORMAT_R32_UINT;
        case Format::R32Sint:           return VK_FORMAT_R32_SINT;
        case Format::R32Float:          return VK_FORMAT_R32_SFLOAT;
        case Format::R32G32Uint:        return VK_FORMAT_R32G32_UINT;
        case Format::R32G32Sint:        return VK_FORMAT_R32G32_SINT;
        case Format::R32G32Float:       return VK_FORMAT_R32G32_SFLOAT;
        case Format::R32G32B32A32Uint:  return VK_FORMAT_R32G32B32A32_UINT;
        case Format::R32G32B32A32Sint:  return VK_FORMAT_R32G32B32A32_SINT;
        case Format::R32G32B32A32Float: return VK_FORMAT_R32G32B32A32_SFLOAT;
        case Format::D16Unorm:          return VK_FORMAT_D16_UNORM;
        case Format::D24UnormS8Uint:    return VK_FORMAT_D24_UNORM_S8_UINT;
        case Format::D32Sfloat:         return VK_FORMAT_D32_SFLOAT;
        case Format::D32SfloatS8Uint:   return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case Format::Count:             return VK_FORMAT_UNDEFINED;
        default:                        return VK_FORMAT_UNDEFINED;
        }
    }

    // Conversion from VkFormat to Format
    constexpr Format VkFormatToFormat(VkFormat format) noexcept {
        switch (format) {
        case VK_FORMAT_UNDEFINED:                return Format::Undefined;
        case VK_FORMAT_R8_UNORM:                 return Format::R8Unorm;
        case VK_FORMAT_R8_SNORM:                 return Format::R8Snorm;
        case VK_FORMAT_R8_UINT:                  return Format::R8Uint;
        case VK_FORMAT_R8_SINT:                  return Format::R8Sint;
        case VK_FORMAT_R8G8_UNORM:               return Format::R8G8Unorm;
        case VK_FORMAT_R8G8_SNORM:               return Format::R8G8Snorm;
        case VK_FORMAT_R8G8_UINT:                return Format::R8G8Uint;
        case VK_FORMAT_R8G8_SINT:                return Format::R8G8Sint;
        case VK_FORMAT_R8G8B8A8_UNORM:           return Format::R8G8B8A8Unorm;
        case VK_FORMAT_R8G8B8A8_SNORM:           return Format::R8G8B8A8Snorm;
        case VK_FORMAT_R8G8B8A8_UINT:            return Format::R8G8B8A8Uint;
        case VK_FORMAT_R8G8B8A8_SINT:            return Format::R8G8B8A8Sint;
        case VK_FORMAT_R8G8B8A8_SRGB:            return Format::R8G8B8A8Srgb;
        case VK_FORMAT_B8G8R8A8_UNORM:           return Format::B8G8R8A8Unorm;
        case VK_FORMAT_B8G8R8A8_SRGB:            return Format::B8G8R8A8Srgb;
        case VK_FORMAT_A2R10G10B10_UNORM_PACK32: return Format::A2R10G10B10Unorm;
        case VK_FORMAT_R16_SFLOAT:               return Format::R16Float;
        case VK_FORMAT_R16_UINT:                 return Format::R16Uint;
        case VK_FORMAT_R16_SINT:                 return Format::R16Sint;
        case VK_FORMAT_R16G16_SFLOAT:            return Format::R16G16Float;
        case VK_FORMAT_R16G16_UINT:              return Format::R16G16Uint;
        case VK_FORMAT_R16G16_SINT:              return Format::R16G16Sint;
        case VK_FORMAT_R16G16B16A16_SFLOAT:      return Format::R16G16B16A16Float;
        case VK_FORMAT_R16G16B16A16_UINT:        return Format::R16G16B16A16Uint;
        case VK_FORMAT_R16G16B16A16_SINT:        return Format::R16G16B16A16Sint;
        case VK_FORMAT_R32_UINT:                 return Format::R32Uint;
        case VK_FORMAT_R32_SINT:                 return Format::R32Sint;
        case VK_FORMAT_R32_SFLOAT:               return Format::R32Float;
        case VK_FORMAT_R32G32_UINT:              return Format::R32G32Uint;
        case VK_FORMAT_R32G32_SINT:              return Format::R32G32Sint;
        case VK_FORMAT_R32G32_SFLOAT:            return Format::R32G32Float;
        case VK_FORMAT_R32G32B32A32_UINT:        return Format::R32G32B32A32Uint;
        case VK_FORMAT_R32G32B32A32_SINT:        return Format::R32G32B32A32Sint;
        case VK_FORMAT_R32G32B32A32_SFLOAT:      return Format::R32G32B32A32Float;
        case VK_FORMAT_D16_UNORM:                return Format::D16Unorm;
        case VK_FORMAT_D24_UNORM_S8_UINT:        return Format::D24UnormS8Uint;
        case VK_FORMAT_D32_SFLOAT:               return Format::D32Sfloat;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:       return Format::D32SfloatS8Uint;
        default:                                 return Format::Undefined;
        }
    }
}