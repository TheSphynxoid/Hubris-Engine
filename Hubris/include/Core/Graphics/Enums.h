#pragma once
#include "Format.h"
#include "Core/Utils.h"

namespace Hubris::Graphics
{
    enum class ImageLayout {
        Undefined,
        ColorAttachment,
        DepthStencilAttachment,
        ReadOnly,
        CopySrc,
        CopyDest
    };

    enum class ImageMemoryType {
        GPU_LOCAL,
        CPU_VISIBLE,
        CPU_TO_GPU
    };

	enum class Swizzle : unsigned char {
        X = 0x0001, Y = 0x0002, Z = 0x0004, W = 0x0010,
        R=X, G=Y, B=Z, A=W
    };
    ENABLE_ENUM_FLAGS(Swizzle);

    enum class PipelineType : unsigned char {
		Graphics,
		Compute,
		RayTracing,
	};

	/**
	 * @brief Hubris' PrimitiveTopology is currently a 1:1 of vulkan's enum. 
	 * 
	 * Use ToVkPrimitiveTopology() in Vulkan/Utility header in case the engine (or vulkan) ever breaks the matching with vulkan.
	 * If ToVkPrimitiveTopology() causes an error, That is a bug that must be fixed (in the switch case).
	 */
	enum class PrimitiveTopology : unsigned char {
		PointList = 0,
		LineList = 1,
		LineStrip = 2,
		TriangleList = 3,
		TriangleStrip = 4,
		TriangleFan = 5,
		ListLineWithAdjacency = 6,
		LineStripWithAdjacency = 7,
		TriangleListWithAdjacency = 8,
		TriangleStripWithAdjacency = 9,
		PatchList = 10,
		Count
	};

	enum class PolygonMode {
		Fill, 
		Line, ///< Requires a GPU features, beware.
		Point ///< Requires a GPU features, beware.
	};

	enum class CullMode {
		None,
		Front,
		Back,
		FrontAndBack,
	};

	enum class FrontFaceOrder {
		CounterClockwise,
		Clockwise,
	};


	enum class SampleCount{
		Count1 = 1,
		Count2 = 2,
		Count4 = 4,
		Count8 = 8,
		Count16 = 16,
		Count32 = 32,
		// Count64 = 0x00000040, //This is just left in case count 64 becomes general.
	};

    enum class PresentMode : unsigned char {
        Immediate, ///< DX12 maps it to FLIP
        Mailbox, ///< DX12 maps it to FLIP_DISCARD
        Fifo, ///< These are mapped to vsync-locked modes (TODO: research details about DX12 equiv.)
        FifoRelaxed ///< These are mapped to vsync-locked modes (TODO: research details about DX12 equiv.)
    };

	//This is mainly for engine state managment and swapchain rebuilding.
    enum class SwapchainResult : unsigned char {
        Success,
        Timeout,
        Suboptimal,
        OutOfDate,
        Error
    };
} // namespace Hubris::Graphics
