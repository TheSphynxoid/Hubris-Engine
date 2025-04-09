#pragma once
#include "Error.h"

namespace Sphynx::Graphics::GL {
    class GLPlatform {
    public:
        ErrorCode InitGL();
    };
}