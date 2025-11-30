#pragma once
#include "Core/Graphics/Shader.h"

namespace Hubris::Graphics {
	struct ShaderPack {
		Shader* Vertex;
		Shader* Fragment;
	};

	
	enum class PipelineType {
		Graphics,
		Compute,
		RayTracing,
	};

	struct PipelineDescriptor {
    	PipelineType type;
    	std::vector<Handle<Shader>> shaders;
		
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
		static Handle<Pipeline> Create(const PipelineDescriptor& shaders);
	};
}