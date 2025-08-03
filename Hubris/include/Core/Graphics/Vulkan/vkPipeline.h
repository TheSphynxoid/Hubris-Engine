#pragma once
#include "Core/Graphics/Pipeline.h"

namespace Hubris::Graphics
{
    class vkPipeline final : public Pipeline{
    private:
        
    public:
        vkPipeline(const PipelineDescriptor& desc);
        ~vkPipeline();
        
    };
} // namespace Hubris::Graphics

