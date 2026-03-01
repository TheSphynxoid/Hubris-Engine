#pragma once
#include "glm/glm.hpp"


namespace Hubris::Graphics
{
    /**
     * @brief Screen-space Dimensions. The Depth Values are used by the pipelines 
     */
    struct Viewport {
        float Width;
        float Height;
        float x;
        float y;
        float MinDepth;
        float MaxDepth;
    };

    struct Extent{
        uint32_t Width;
        uint32_t Height;
    };

    struct Rect{
        glm::ivec2 Offset;
        Extent Extent;
    };
} // namespace Sphynx::Graphics
