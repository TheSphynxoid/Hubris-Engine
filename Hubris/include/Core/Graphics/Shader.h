#pragma once
#include "Core/Graphics/Format.h"
#define ENUMSHIFT(n) (0x1 << n)


namespace Hubris::Graphics {
	enum class ShaderStage {
		Unknown, 
		Vertex = ENUMSHIFT(0),
		Fragment = ENUMSHIFT(1),
		Geometry = ENUMSHIFT(3),
		TessellationControl = ENUMSHIFT(4),
		TessellationEvaluation = ENUMSHIFT(5),
		Compute = ENUMSHIFT(6),
	};

	class Shader {
	protected:
		ShaderStage stage = ShaderStage::Unknown;
		Shader(const ShaderStage& type) noexcept : stage(type) {};
	public:
		virtual void Destroy()noexcept = 0;
		/**
		 * @brief Override if you have an implementation specific validation method, this simply checks if the stage is not ShaderStage::Unknown.
		 */
		virtual bool Valid()const noexcept {
			return stage != ShaderStage::Unknown;
		};
		static Handle<Shader> Create(const std::vector<char>& data, ShaderStage type);
	};

}