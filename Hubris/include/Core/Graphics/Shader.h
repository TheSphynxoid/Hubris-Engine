#pragma once
#include "Core/Graphics/Format.h"
#include "../../Memory.h"
#define ENUMSHIFT(n) (0x1 << n)


namespace Hubris::Graphics {
	enum class ShaderStage {
		Unknown, 
		Vertex = ENUMSHIFT(0),
		Fragment = ENUMSHIFT(1),
		Geometry = ENUMSHIFT(2),
		TessellationControl = ENUMSHIFT(3),
		TessellationEvaluation = ENUMSHIFT(4),
		Compute = ENUMSHIFT(5),
	};
		
	constexpr const char* VertexStageToString(const Hubris::Graphics::ShaderStage stage) noexcept {
		switch (stage)
		{
		case ShaderStage::Vertex:
			return "Vertex";
		case ShaderStage::Fragment:
			return "Fragment";
		case ShaderStage::Geometry:
			return "Geometry";
		case ShaderStage::TessellationControl:
			return "Tessellation Control";
		case ShaderStage::TessellationEvaluation:
			return "Tessallation Evaluation";
		case ShaderStage::Compute:
			return "Compute";
		case ShaderStage::Unknown:
			__fallthrough;
		default:
			return "Error/Unknown";
		}
	}

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