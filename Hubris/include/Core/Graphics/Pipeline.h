#pragma once
#include "Core/Utils.h"
#include "Core/Graphics/Enums.h"
#include "Core/Graphics/Shader.h"

namespace Hubris::Graphics {	
	/**
	 * @brief Configuration for the Rasterizer.
	 * 
	 * (DepthClip is the opposite of DepthClamp meaning, 
	 * if DepthClip is false Geometry outside the near/far planes are clamped and if true, they are clipped)
	 * 
	 * @Assumptions Assumptions are (currently i don't have an override system for these, but i want to make one) default values.
	 * 
	 * For vulkan, The Engine will assume:
	 * - The engine will attempt to enable DepthClamp without explicit Specification.
	 * - LineWidth as 1.0f (The engine will not enable the wideLines feature by default)
	 * 
	 * Compatibility Assumptions:
	 * - DX12 Assumes FrontFaceOrder is Clockwise by default. The Vk backend will also assume that for the wrong/invalid Enum values.
	 */
	struct Rasterizer {
		PolygonMode polygoneMode = PolygonMode::Fill;
		FrontFaceOrder frontFace = FrontFaceOrder::Clockwise;
		float depthBiasConstantFactor = .0f, depthBiasClamp = .0f, depthBiasSlopeFactor = .0f;
		bool discard = false;
		bool depthBias = false;
		bool DepthClip = true;
	} const DefaultRaster = Rasterizer();

	struct MultiSamplingConfig {
		SampleCount sampleCount = SampleCount::Count1;
		bool perSampleShading = false;
		//TODO: Add a SampleMask Abstraction.
		//SampleMask* mask = nullptr;
	};

	//TODO: add depth/Astencil buffer support.
	/**
	 * @brief Abstraction of the pipeline descriptor.
	 * 
	 * @Assumptions
	 * 
	 * PipelineDescriptor::primitiveRestartEnable: DX12 implicitly enables this for strip topologies; Vulkan requires explicit flag.
	 * 
	 */
	struct PipelineDescriptor {
    	PipelineType type;
		PrimitiveTopology topology = PrimitiveTopology::TriangleList;
		uint8_t patchControlPoints = 0; ///< For Tessellation and PatchList topology. 
		bool primitiveRestartEnable = false;  ///< For Strip topology, DX12 has this implicitly set to true. Backend must handle.
    	std::vector<Handle<Shader>> shaders;
		Rasterizer rasterizeConfig = DefaultRaster; ///< Assigned the default rasterize
		MultiSamplingConfig multiSampleConfig = MultiSamplingConfig();
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