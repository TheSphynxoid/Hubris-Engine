#pragma once
#include "Core/Graphics/Format.h"
#include "../../Memory.h"
#include <span>
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
	    virtual ~Shader() = default;
		virtual void Destroy()noexcept = 0;
		/**
		 * @brief Override if you have an implementation specific validation method, this simply checks if the stage is not ShaderStage::Unknown.
		 */
		virtual bool Valid()const noexcept {
			return stage != ShaderStage::Unknown;
		};
		static Handle<Shader> Create(const std::span<const uint8_t>& data, ShaderStage type, std::string entryPoint = "main");

		/// @brief Deduce the ShaderStage from the SPIR-V binary itself (authoritative).
		/// Reads OpEntryPoint's execution model. Minimal reader; Phase 2's full
		/// spirv-reflect pass supersedes this (same source of truth, richer extraction).
		/// @return The stage, or Unknown if the data isn't valid SPIR-V.
		static ShaderStage StageFromSpirv(std::span<const uint8_t> data) noexcept;

		/// @brief Type-erased factory for ResourceManager. Reads the stage from the
		/// SPIR-V binary (authoritative), calls Create, and releases the object out of
		/// its Handle as void* (the ResourceManager cache takes ownership). Returns
		/// nullptr on failure.
		static void* Factory(std::span<const uint8_t> data, std::string_view path);

		/// @brief Destructor companion for Factory — calls `delete` on the typed object.
		/// Safe on nullptr.
		static void CacheDestructor(void* obj) noexcept;
	};

	/// @brief Register the graphics layer's typed factories with the ResourceManager.
	/// Called once by Engine::Init after ResourceManager::InitializeDefaults(). Keeps
	/// the dependency direction honest: Graphics -> RM (registers), RM -> VFS (mounts).
	void RegisterAssetFactories();

}