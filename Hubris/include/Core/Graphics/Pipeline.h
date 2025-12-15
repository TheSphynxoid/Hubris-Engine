#pragma once
#include "Core/Graphics/Shader.h"

//TODO: Maybe I should move the enums to a different header but they are specific so.

namespace Hubris::Graphics {	
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
		Count = 0x7FFFFFFF
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

	/**
	 * @brief Configuration for the Rasterizer.
	 * 
	 * @Assumptions Assumptions are (currently i don't have an override system for these, but i want to make one) values.
	 * 
	 * For vulkan, The Engine will assume:
	 * - The engine will attempt to enable DepthClamp without explicit Specification.
	 * - DepthClamp will be assumed as false by default.
	 * - LineWidth as 1.0f (The engine will not enable the wideLines feature by default)
	 * 
	 * Compatibility Assumptions:
	 * - DX12 Assumes FrontFaceOrder is Clockwise by default. The Vk backend will also assume that for the wrong/invalid Enum values.
	 * - TODO: Read about how DX12 handles Multisampling, i think it isn't the same as Vk. Also, DX12 doesn't support 64bit Multisampling (i think)
	 */
	struct Rasterizer {
		PolygonMode polygoneMode = PolygonMode::Fill;
		FrontFaceOrder frontFace = FrontFaceOrder::Clockwise;
		bool discard = false;
		bool depthBias = false;
		float depthBiasConstantFactor = .0f, depthBiasClamp = .0f, depthBiasSlopeFactor = .0f;
	} const DefaultRaster = Rasterizer();

	enum class SampleCount{
		SampleCount_1bit = 0x00000001,
		SampleCount_2bit = 0x00000002,
		SampleCount_4bit = 0x00000004,
		SampleCount_8bit = 0x00000008,
		SampleCount_16bit = 0x00000010,
		SampleCount_32bit = 0x00000020,
		SampleCount_64bit = 0x00000040,
	};

	struct MultiSamplingConfig {
		bool sampleShading = false;
		SampleCount sampleCount = SampleCount::SampleCount_1bit;
		float minSampleShading = .0f;
		//SampleMask* mask = nullptr;
	};

	//TODO: add depth/stencil buffer support.
	struct PipelineDescriptor {
    	PipelineType type;
		PrimitiveTopology topology = PrimitiveTopology::TriangleList;
		bool primitiveRestartEnable = false; 
    	std::vector<Handle<Shader>> shaders;
		Rasterizer rasterizeConfig = DefaultRaster; ///< Assigned the default rasterize
		// Additional config:
		// - Vertex input layout
		// - Blend state
		// - Depth/stencil state
		// - Rasterizer state
		// - Push constants layout
		// - Descriptor set layouts
	};
	class Pipeline {
	public:
		static Handle<Pipeline> Create(const PipelineDescriptor& desc);
	};
}