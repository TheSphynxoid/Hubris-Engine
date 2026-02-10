#pragma once
#include "Core/Utils.h"
#include "Core/Graphics/Enums.h"
#include "Core/Graphics/Structs.h"
#include "Core/Graphics/Shader.h"

namespace Hubris::Graphics {	
	/**
	 * @brief Configuration for the Rasterizer.
	 * 
	 * (DepthClip is the opposite of DepthClamp meaning, 
	 * if DepthClip is false Geometry outside the near/far planes are clamped and if true, they are clipped)
	 * 
	 * (I may add a system that allows specifying backend specific flags as an override)
	 * @assumption{Vulkan}
	 * - The engine will attempt to enable DepthClamp without explicit Specification.
	 * On fail:
	 * 		- The Engine will log the issue per pipeline creation, then ignore all false values of DepthClip and treat the value as true..
	 * 		- (TODO: Add a severity flag) If Severe is enabled, the engine will throw. This will not be avaible at game runtime.
	 * - LineWidth as 1.0f (The engine will not enable the wideLines feature by default)
	 * 
	 * @assumption{Compatibility}
	 * - DX12 Assumes FrontFaceOrder is Clockwise by default. The Vk backend will discard wrong/invalid Enum values and assume Clockwise.
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

	struct ColorBlendAttachment {
		Component colorWriteMask = AllComponents;
		bool blendEnable = false;
	};

	//TODO: add depth/Astencil buffer support.
	/**
	 * @brief Abstraction of the pipeline descriptor.
	 * 
	 * @assumption{Compatibility}
	 * PipelineDescriptor::primitiveRestartEnable: DX12 implicitly enables this for strip topologies; Vulkan requires explicit flag.
	 * 
	 */
	struct PipelineDescriptor {
		Viewport viewport;
		Rect scissor;
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