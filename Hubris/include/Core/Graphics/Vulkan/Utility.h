#pragma once
#include "../Format.h"
#include "../Shader.h"
#include "../Pipeline.h"
#include "volk.h"

namespace Hubris::Graphics::Vulkan
{
    inline constexpr VkShaderStageFlagBits VkShaderStageToFlags(const ShaderStage &stage) noexcept
    {
        switch (stage)
        {
        case ShaderStage::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderStage::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderStage::Geometry:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case ShaderStage::TessellationControl:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case ShaderStage::TessellationEvaluation:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        case ShaderStage::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            return static_cast<VkShaderStageFlagBits>(0);
        }
    }

    inline constexpr VkPrimitiveTopology ToVkPrimitiveTopology(const PrimitiveTopology &topology) noexcept
    {
        /** Hubris' PrimitiveTopology is currently a 1:1 of vulkan's enum. */
        /**This function Simply Converts by casting, It exists in case the engine breaks compatibility with vulkan. */
        return (VkPrimitiveTopology)topology;
    }

    // Conversion from Format to VkFormat
    constexpr VkFormat FormatToVkFormat(Format format) noexcept
    {
        switch (format)
        {
        case Format::Undefined:
            return VK_FORMAT_UNDEFINED;
        case Format::R8Unorm:
            return VK_FORMAT_R8_UNORM;
        case Format::R8Snorm:
            return VK_FORMAT_R8_SNORM;
        case Format::R8Uint:
            return VK_FORMAT_R8_UINT;
        case Format::R8Sint:
            return VK_FORMAT_R8_SINT;
        case Format::R8G8Unorm:
            return VK_FORMAT_R8G8_UNORM;
        case Format::R8G8Snorm:
            return VK_FORMAT_R8G8_SNORM;
        case Format::R8G8Uint:
            return VK_FORMAT_R8G8_UINT;
        case Format::R8G8Sint:
            return VK_FORMAT_R8G8_SINT;
        case Format::R8G8B8A8Unorm:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case Format::R8G8B8A8Snorm:
            return VK_FORMAT_R8G8B8A8_SNORM;
        case Format::R8G8B8A8Uint:
            return VK_FORMAT_R8G8B8A8_UINT;
        case Format::R8G8B8A8Sint:
            return VK_FORMAT_R8G8B8A8_SINT;
        case Format::R8G8B8A8Srgb:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case Format::B8G8R8A8Unorm:
            return VK_FORMAT_B8G8R8A8_UNORM;
        case Format::B8G8R8A8Srgb:
            return VK_FORMAT_B8G8R8A8_SRGB;
        case Format::A2R10G10B10Unorm:
            return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
        case Format::R16Float:
            return VK_FORMAT_R16_SFLOAT;
        case Format::R16Uint:
            return VK_FORMAT_R16_UINT;
        case Format::R16Sint:
            return VK_FORMAT_R16_SINT;
        case Format::R16G16Float:
            return VK_FORMAT_R16G16_SFLOAT;
        case Format::R16G16Uint:
            return VK_FORMAT_R16G16_UINT;
        case Format::R16G16Sint:
            return VK_FORMAT_R16G16_SINT;
        case Format::R16G16B16A16Float:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case Format::R16G16B16A16Uint:
            return VK_FORMAT_R16G16B16A16_UINT;
        case Format::R16G16B16A16Sint:
            return VK_FORMAT_R16G16B16A16_SINT;
        case Format::R32Uint:
            return VK_FORMAT_R32_UINT;
        case Format::R32Sint:
            return VK_FORMAT_R32_SINT;
        case Format::R32Float:
            return VK_FORMAT_R32_SFLOAT;
        case Format::R32G32Uint:
            return VK_FORMAT_R32G32_UINT;
        case Format::R32G32Sint:
            return VK_FORMAT_R32G32_SINT;
        case Format::R32G32Float:
            return VK_FORMAT_R32G32_SFLOAT;
        case Format::R32G32B32A32Uint:
            return VK_FORMAT_R32G32B32A32_UINT;
        case Format::R32G32B32A32Sint:
            return VK_FORMAT_R32G32B32A32_SINT;
        case Format::R32G32B32A32Float:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case Format::D16Unorm:
            return VK_FORMAT_D16_UNORM;
        case Format::D24UnormS8Uint:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case Format::D32Sfloat:
            return VK_FORMAT_D32_SFLOAT;
        case Format::D32SfloatS8Uint:
            return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case Format::Count:
            return VK_FORMAT_UNDEFINED;
        default:
            return VK_FORMAT_UNDEFINED;
        }
    }

    // Conversion from VkFormat to Format
    constexpr Format VkFormatToFormat(VkFormat format) noexcept
    {
        switch (format)
        {
        case VK_FORMAT_UNDEFINED:
            return Format::Undefined;
        case VK_FORMAT_R8_UNORM:
            return Format::R8Unorm;
        case VK_FORMAT_R8_SNORM:
            return Format::R8Snorm;
        case VK_FORMAT_R8_UINT:
            return Format::R8Uint;
        case VK_FORMAT_R8_SINT:
            return Format::R8Sint;
        case VK_FORMAT_R8G8_UNORM:
            return Format::R8G8Unorm;
        case VK_FORMAT_R8G8_SNORM:
            return Format::R8G8Snorm;
        case VK_FORMAT_R8G8_UINT:
            return Format::R8G8Uint;
        case VK_FORMAT_R8G8_SINT:
            return Format::R8G8Sint;
        case VK_FORMAT_R8G8B8A8_UNORM:
            return Format::R8G8B8A8Unorm;
        case VK_FORMAT_R8G8B8A8_SNORM:
            return Format::R8G8B8A8Snorm;
        case VK_FORMAT_R8G8B8A8_UINT:
            return Format::R8G8B8A8Uint;
        case VK_FORMAT_R8G8B8A8_SINT:
            return Format::R8G8B8A8Sint;
        case VK_FORMAT_R8G8B8A8_SRGB:
            return Format::R8G8B8A8Srgb;
        case VK_FORMAT_B8G8R8A8_UNORM:
            return Format::B8G8R8A8Unorm;
        case VK_FORMAT_B8G8R8A8_SRGB:
            return Format::B8G8R8A8Srgb;
        case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
            return Format::A2R10G10B10Unorm;
        case VK_FORMAT_R16_SFLOAT:
            return Format::R16Float;
        case VK_FORMAT_R16_UINT:
            return Format::R16Uint;
        case VK_FORMAT_R16_SINT:
            return Format::R16Sint;
        case VK_FORMAT_R16G16_SFLOAT:
            return Format::R16G16Float;
        case VK_FORMAT_R16G16_UINT:
            return Format::R16G16Uint;
        case VK_FORMAT_R16G16_SINT:
            return Format::R16G16Sint;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return Format::R16G16B16A16Float;
        case VK_FORMAT_R16G16B16A16_UINT:
            return Format::R16G16B16A16Uint;
        case VK_FORMAT_R16G16B16A16_SINT:
            return Format::R16G16B16A16Sint;
        case VK_FORMAT_R32_UINT:
            return Format::R32Uint;
        case VK_FORMAT_R32_SINT:
            return Format::R32Sint;
        case VK_FORMAT_R32_SFLOAT:
            return Format::R32Float;
        case VK_FORMAT_R32G32_UINT:
            return Format::R32G32Uint;
        case VK_FORMAT_R32G32_SINT:
            return Format::R32G32Sint;
        case VK_FORMAT_R32G32_SFLOAT:
            return Format::R32G32Float;
        case VK_FORMAT_R32G32B32A32_UINT:
            return Format::R32G32B32A32Uint;
        case VK_FORMAT_R32G32B32A32_SINT:
            return Format::R32G32B32A32Sint;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return Format::R32G32B32A32Float;
        case VK_FORMAT_D16_UNORM:
            return Format::D16Unorm;
        case VK_FORMAT_D24_UNORM_S8_UINT:
            return Format::D24UnormS8Uint;
        case VK_FORMAT_D32_SFLOAT:
            return Format::D32Sfloat;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return Format::D32SfloatS8Uint;
        default:
            return Format::Undefined;
        }
    }

    constexpr VkPolygonMode PolygonModeToVK(PolygonMode mode)
    {
        switch (mode)
        {
        case PolygonMode::Fill:
            return VkPolygonMode::VK_POLYGON_MODE_FILL;
        case PolygonMode::Line:
            return VkPolygonMode::VK_POLYGON_MODE_LINE;
        case PolygonMode::Point:
            return VkPolygonMode::VK_POLYGON_MODE_POINT;
        default:
            return VkPolygonMode::VK_POLYGON_MODE_MAX_ENUM;
        }
    }

    constexpr VkCullModeFlagBits CullModeToVk(CullMode cull)
    {
        switch (cull)
        {
        case CullMode::None:
            return VkCullModeFlagBits::VK_CULL_MODE_NONE;
        case CullMode::Front:
            return VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
        case CullMode::Back:
            return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
        case CullMode::FrontAndBack:
            return VkCullModeFlagBits::VK_CULL_MODE_FRONT_AND_BACK;
        default:
            return VkCullModeFlagBits::VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
        }
    }

    constexpr VkFrontFace FrontFaceOrderToVk(FrontFaceOrder face)
    {
        switch (face)
        {
        case FrontFaceOrder::CounterClockwise:
            return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
        case FrontFaceOrder::Clockwise:
            return VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
        default:
            return VkFrontFace::VK_FRONT_FACE_MAX_ENUM;
        }
    }

    constexpr VkSampleCountFlagBits SampleCountToVk(SampleCount count)
    {
        switch (count)
        {
        case SampleCount::Count1:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
        case SampleCount::Count2:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_2_BIT;
        case SampleCount::Count4:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_4_BIT;
        case SampleCount::Count8:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_8_BIT;
        case SampleCount::Count16:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_16_BIT;
        case SampleCount::Count32:
            return VkSampleCountFlagBits::VK_SAMPLE_COUNT_32_BIT;
        default:
            return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
        }
    }

    constexpr VkBlendOp BlendOpToVk(BlendOp op)
    {
        switch (op)
        {
        case BlendOp::Add:
            return VK_BLEND_OP_ADD;
        case BlendOp::Subtract:
            return VK_BLEND_OP_SUBTRACT;
        case BlendOp::RevSubtract:
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        case BlendOp::Min:
            return VK_BLEND_OP_MIN;
        case BlendOp::Max:
            return VK_BLEND_OP_MAX;
        default:
            return VK_BLEND_OP_MAX_ENUM;
        }
    }

    constexpr VkLogicOp LogicOpToVk(LogicOp op)
    {
        switch (op)
        {
        case LogicOp::Clear:
            return VK_LOGIC_OP_CLEAR;
        case LogicOp::AND:
            return VK_LOGIC_OP_AND;
        case LogicOp::AND_Reverse:
            return VK_LOGIC_OP_AND_REVERSE;
        case LogicOp::Copy:
            return VK_LOGIC_OP_COPY;
        case LogicOp::AND_Inverted:
            return VK_LOGIC_OP_AND_INVERTED;
        case LogicOp::NoOP:
            return VK_LOGIC_OP_NO_OP;
        case LogicOp::XOR:
            return VK_LOGIC_OP_XOR;
        case LogicOp::OR:
            return VK_LOGIC_OP_OR;
        case LogicOp::NOR:
            return VK_LOGIC_OP_NOR;
        case LogicOp::Equivalent:
            return VK_LOGIC_OP_EQUIVALENT;
        case LogicOp::Invert:
            return VK_LOGIC_OP_INVERT;
        case LogicOp::OR_Reverse:
            return VK_LOGIC_OP_OR_REVERSE;
        case LogicOp::CopyInverted:
            return VK_LOGIC_OP_COPY_INVERTED;
        case LogicOp::OR_Inverted:
            return VK_LOGIC_OP_OR_INVERTED;
        case LogicOp::NAND:
            return VK_LOGIC_OP_NAND;
        case LogicOp::Set:
            return VK_LOGIC_OP_SET;
        default:
            return VK_LOGIC_OP_MAX_ENUM;
        }
    }

    constexpr VkBlendFactor BlendFactorToVk(BlendFactor factor)
    {
        switch (factor)
        {
        case BlendFactor::Zero:
            return VK_BLEND_FACTOR_ZERO;
        case BlendFactor::One:
            return VK_BLEND_FACTOR_ONE;
        case BlendFactor::SrcColor:
            return VK_BLEND_FACTOR_SRC_COLOR;
        case BlendFactor::OneMinusSrcColor:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DstColor:
            return VK_BLEND_FACTOR_DST_COLOR;
        case BlendFactor::OneMinusDstColor:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case BlendFactor::SrcAlpha:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendFactor::OneMinusSrcAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DstAlpha:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case BlendFactor::OneMinusDstAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case BlendFactor::ConstantColor:
            return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case BlendFactor::OneMinusConstantColor:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case BlendFactor::ConstantAlpha:
            return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case BlendFactor::OneMinusConstantAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        case BlendFactor::SrcAlphaSaturate:
            return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        case BlendFactor::Src1Color:
            return VK_BLEND_FACTOR_SRC1_COLOR;
        case BlendFactor::OneMinusSrc1Color:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        case BlendFactor::Src1Alpha:
            return VK_BLEND_FACTOR_SRC1_ALPHA;
        case BlendFactor::OneMinusSrc1Alpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        default:
            return VK_BLEND_FACTOR_MAX_ENUM;
        }
    }

    constexpr VkColorComponentFlags ColorComponentFlagToVK(Component comp)
    {
        VkColorComponentFlags flags = 0;

        if ((ToUnderlying(comp) & ToUnderlying(Component::X)) != 0)
            flags |= VK_COLOR_COMPONENT_R_BIT;
        if ((ToUnderlying(comp) & ToUnderlying(Component::Y)) != 0)
            flags |= VK_COLOR_COMPONENT_G_BIT;
        if ((ToUnderlying(comp) & ToUnderlying(Component::Z)) != 0)
            flags |= VK_COLOR_COMPONENT_B_BIT;
        if ((ToUnderlying(comp) & ToUnderlying(Component::W)) != 0)
            flags |= VK_COLOR_COMPONENT_A_BIT;

        return flags;
    }

    
}