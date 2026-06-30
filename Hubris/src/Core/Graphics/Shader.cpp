#include "pch.h"
#include "Core/Graphics/Shader.h"
#include "Core/Graphics/Vulkan/vkShader.h"
#include "IO/ResourceManager.h"

using namespace Hubris;

Handle<Graphics::Shader> Graphics::Shader::Create(const std::span<const uint8_t>& data, ShaderStage type,
    std::string entryPoint)
{
    if constexpr (Platform == Hbr_Platform::Windows) {
        return new Vulkan::VulkanShader(data, type, entryPoint);
    }
    assert(false && "Shader::Create: platform unsupported or unknown");
    return nullptr;
}

Graphics::ShaderStage Graphics::Shader::StageFromSpirv(std::span<const uint8_t> data) noexcept {
    // Minimal SPIR-V reader: walk instructions until OpEntryPoint, read the execution
    // model. The binary is authoritative (the compiler wrote it). No deps; Phase 2's full
    // spirv-reflect pass supersedes this but reads the same authoritative field.
    //
    // SPIR-V layout:
    //   word 0: magic  0x07230203
    //   word 1: version
    //   word 2: generator magic
    //   word 3: bound (IDs)
    //   word 4: reserved (0)
    //   word 5+: instructions, each: word 0 = (word_count << 16) | opcode
    //
    // OpEntryPoint (op=15) layout:
    //   word 0: instruction header
    //   word 1: Execution Model
    //   word 2: Entry Point <id>
    //   word 3: ... (name string, then interface <id>s)
    if (data.size() < 20) return ShaderStage::Unknown; // need at least the 5-word header
    const uint32_t* words = reinterpret_cast<const uint32_t*>(data.data());
    // Alignment: SPIR-V requires 4-byte alignment. Span from VFS may not guarantee it,
    // but the FileSystemLoader buffer is new[]'d uint8_ts — check alignment defensively.
    if (reinterpret_cast<uintptr_t>(words) % alignof(uint32_t) != 0) {
        return ShaderStage::Unknown;
    }
    constexpr uint32_t SPIRV_MAGIC = 0x07230203;
    if (words[0] != SPIRV_MAGIC) return ShaderStage::Unknown;

    const size_t wordCount = data.size() / sizeof(uint32_t);
    size_t i = 5; // skip the 5-word header
    while (i < wordCount) {
        const uint32_t instr = words[i];
        const uint32_t opcode = instr & 0xFFFF;
        const uint32_t wc = instr >> 16;
        if (wc == 0) break; // malformed
        if (opcode == 15) { // OpEntryPoint
            if (i + 1 < wordCount) {
                const uint32_t execModel = words[i + 1];
                switch (execModel) {
                    case 0: return ShaderStage::Vertex;
                    case 1: return ShaderStage::TessellationControl;
                    case 2: return ShaderStage::TessellationEvaluation;
                    case 3: return ShaderStage::Geometry;
                    case 4: return ShaderStage::Fragment;
                    case 5: return ShaderStage::Compute;
                    default: return ShaderStage::Unknown; // ray/mesh/task — Phase 2.
                }
            }
            return ShaderStage::Unknown;
        }
        i += wc;
    }
    return ShaderStage::Unknown;
}

void* Hubris::Graphics::Shader::Factory(std::span<const uint8_t> data, std::string_view path) {
    // Authoritative: read the stage from the SPIR-V binary itself.
    const ShaderStage stage = StageFromSpirv(data);
    if (stage == ShaderStage::Unknown) {
        Logger::Log("Shader::Factory: could not deduce stage from SPIR-V for '{}'", path);
        return nullptr;
    }
    Handle<Shader> handle = Shader::Create(data, stage);
    if (!handle) return nullptr;
    return handle.release(); // transfer ownership to the ResourceManager cache
}

void Hubris::Graphics::Shader::CacheDestructor(void* obj) noexcept {
    delete static_cast<Shader*>(obj); // delete nullptr is a no-op; virtual dtor handles it.
}

void Hubris::Graphics::RegisterAssetFactories() {
    IO::ResourceManager::Instance().RegisterFactory<Shader>(&Shader::Factory, &Shader::CacheDestructor);
}
