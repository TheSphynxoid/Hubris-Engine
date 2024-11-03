#pragma once
#include "Error.h"

namespace Sphynx::Graphics{
    class GLPlatform {
    public:
        ErrorCode InitGL();
    };
}