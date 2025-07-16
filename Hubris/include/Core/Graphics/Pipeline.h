#pragma once
#include "Shader.h"

namespace Hubris::Graphics {
	struct ShaderPack {
		Shader* Vertex;
		Shader* Fragment;
	};

	class Pipeline {
	public:
		static Pipeline* Create(const ShaderPack& shaders);
	};
}