#pragma once
#include "Error.h"

namespace Hubris::Graphics::GL {
    class GLPlatform {
    public:
        ErrorCode InitGL();
    };
}