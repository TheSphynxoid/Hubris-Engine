#pragma once
#include <array>
#include "Core/Utils.h"
#include "Core/Graphics/Enums.h"
#include "Core/Graphics/Structs.h"
#include "Core/Graphics/Shader.h"
#include "Core/Graphics/PipelineLayout.h"

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
		PolygonMode polygonMode = PolygonMode::Fill;
		FrontFaceOrder frontFace = FrontFaceOrder::Clockwise;
		CullMode cullMode = CullMode::Front;
		float depthBiasConstantFactor = .0f, depthBiasClamp = .0f, depthBiasSlopeFactor = 1.0f;
		bool discard = false;
		bool depthBias = false;
		bool DepthClip = true;
	} const DefaultRaster = Rasterizer();

	struct MultiSamplingConfig {
		SampleCount sampleCount = SampleCount::Count1;
		bool perSampleShading = false;
		bool enabled = false;
		//TODO: Add a SampleMask Abstraction.
		//SampleMask* mask = nullptr;
	};

	/**
	 * @brief Color blending and logic operations for a single color attachment.
	 *
	 * @assumption{Compatibility}
	 * Scope: Vulkan + DX12 (single render target baseline). 
	 * Difference: Vulkan and DX12 both support blend factors/ops and logic operations, but logic-op and blending should not be enabled at the same time.
	 * Fallback: If both are requested, backend should prioritize logic-op and disable blending (deterministic behavior).
	 * Failure: warn.
	 * Reason: Keeps pipeline state valid across backends while preserving explicit user intent.
	 *
	 * @assumption{Defaults}
	 * Scope: Default value semantics.
	 * Difference: Disabled blending should still have deterministic factors/ops.
	 * Fallback: Opaque defaults are identity blend values (src=One, dst=Zero, op=Add).
	 * Failure: ignore.
	 * Reason: Predictable output and backend-neutral defaults.
	 */
	struct BlendConfig {
		Component colorWriteMask = AllComponents;
		bool blendEnable = false;
		bool logicOpEnable = false;
		BlendOp colorBlendOp = BlendOp::Add;
		BlendOp alphaBlendOp = BlendOp::Add;
		BlendFactor srcColorBlendFactor = BlendFactor::One;
		BlendFactor srcAlphaBlendFactor = BlendFactor::One;
		BlendFactor dstColorBlendFactor = BlendFactor::Zero;
		BlendFactor dstAlphaBlendFactor = BlendFactor::Zero;
		std::array<float, 4> blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f };

		LogicOp logicOp = LogicOp::Copy;

		static constexpr BlendConfig Opaque() noexcept {
			return BlendConfig{};
		}

		static constexpr BlendConfig AlphaBlend() noexcept {
			BlendConfig config{};
			config.blendEnable = true;
			config.srcColorBlendFactor = BlendFactor::SrcAlpha;
			config.dstColorBlendFactor = BlendFactor::OneMinusSrcAlpha;
			config.srcAlphaBlendFactor = BlendFactor::One;
			config.dstAlphaBlendFactor = BlendFactor::OneMinusSrcAlpha;
			return config;
		}

		static constexpr BlendConfig PremultipliedAlphaBlend() noexcept {
			BlendConfig config{};
			config.blendEnable = true;
			config.srcColorBlendFactor = BlendFactor::One;
			config.dstColorBlendFactor = BlendFactor::OneMinusSrcAlpha;
			config.srcAlphaBlendFactor = BlendFactor::One;
			config.dstAlphaBlendFactor = BlendFactor::OneMinusSrcAlpha;
			return config;
		}

		static constexpr BlendConfig Additive() noexcept {
			BlendConfig config{};
			config.blendEnable = true;
			config.srcColorBlendFactor = BlendFactor::One;
			config.dstColorBlendFactor = BlendFactor::One;
			config.srcAlphaBlendFactor = BlendFactor::One;
			config.dstAlphaBlendFactor = BlendFactor::One;
			return config;
		}
	};

	//TODO: add depth/stencil buffer support.
	/**
	 * @brief Abstraction of the pipeline descriptor.
	 * 
	 * @assumption{Compatibility}
	 * Scope: Primitive restart behavior.
	 * Difference: DX12 implicitly enables primitive restart for strip topologies; Vulkan requires explicit primitiveRestartEnable.
	 * Fallback: Backend sets API-native state from this field and may force true for strip topologies when required by API semantics.
	 * Failure: warn.
	 * Reason: Keeps topology behavior portable while preserving backend correctness.
	 *
	 * @assumption{Compatibility}
	 * Scope: Independent blending.
	 * Difference: DX12 requires IndependentBlendEnable flag for per-RT blend states; Vulkan behavior is effectively independent per attachment.
	 * Fallback: Vulkan backend ignores this switch if irrelevant, DX12 backend maps directly.
	 * Failure: ignore.
	 * Reason: One neutral descriptor field keeps intent explicit for all backends.
	 *
	 * @assumption{Compatibility}
	 * Scope: Alpha-to-coverage.
	 * Difference: Alpha-to-coverage is meaningful only when multisampling is active.
	 * Fallback: If MSAA sample count is 1, backend treats alphaToCoverageEnable as false.
	 * Failure: ignore.
	 * Reason: Avoids no-op ambiguity and keeps descriptor simple.
	 */
	struct PipelineDescriptor {
		Viewport viewport;
		Rect scissor;
    	PipelineType type = PipelineType::Graphics;
		PrimitiveTopology topology = PrimitiveTopology::TriangleList;
		uint8_t patchControlPoints = 0; ///< For Tessellation and PatchList topology. 
		bool primitiveRestartEnable = false;  ///< For Strip topology, DX12 has this implicitly set to true. Backend must handle.
    	std::vector<Observer<Shader>> shaders; ///< Non-owning views. Shaders are owned by the ResourceManager cache (engine-lifetime).
		Rasterizer rasterizeConfig = DefaultRaster; ///< Assigned the default rasterize
		MultiSamplingConfig multiSampleConfig = MultiSamplingConfig();
		BlendConfig blendConfig = BlendConfig::Opaque();
		bool alphaToCoverageEnable = false;
		bool independentBlendEnable = true; ///< This is always true for vulkan, thus it is ignored for the backend.
		// Pipeline layout. Default-constructed to null — declaring a PipelineDescriptor
		// must not touch the GPU. When left null, the backend creates an empty default
		// layout inside the pipeline factory. See VulkanPipeline ctor.
		Handle<PipelineLayout> pipelineLayout;
		
		// Additional config:
		// - Vertex input layout
		// - Depth/stencil state
		// - Push constants layout
		// - Descriptor set layouts
	};
	
	class Pipeline {
	public:
		static Handle<Pipeline> Create(const PipelineDescriptor& desc);
		// Pipeline() = default;
        virtual ~Pipeline() = default;
	};
}