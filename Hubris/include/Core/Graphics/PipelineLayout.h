#pragma once
#include <vector>
#include "Core/Graphics/Shader.h"
#include "Core/Graphics/Structs.h"

namespace Hubris::Graphics {

    struct DescriptorSetLayout {
        // Abstracted descriptor set layout info (bindings, types, etc.)
        // Fill in as needed for your engine.
    };

    struct PushConstantRange {
        uint32_t offset;
        uint32_t size;
        ShaderStage stages;
    };

    class PipelineLayout {
    private:
    public:
        virtual ~PipelineLayout() = default;
        static Handle<PipelineLayout> Create(const std::vector<DescriptorSetLayout>& setLayouts,
            const std::vector<PushConstantRange>& pushConstants);

        // Backend handle accessors (VkPipelineLayout, D3D12 root signature, etc.)
        virtual void* GetNative()const noexcept = 0;

        // virtual const std::vector<DescriptorSetLayout>& GetDescriptorSetLayouts() const = 0;
        // virtual const std::vector<PushConstantRange>& GetPushConstantRanges() const = 0;

    };

} // namespace Hubris::Graphics