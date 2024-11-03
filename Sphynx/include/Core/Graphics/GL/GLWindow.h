#pragma once
#include "../Window.h"

namespace Sphynx::Graphics::GL
{
    class GLWindow : public Window{
        virtual void Init();
        virtual void Render()noexcept;
        virtual void Close()noexcept;
        virtual Viewport GetViewport()const noexcept;
        virtual bool IsValid()const noexcept;
    };
} // namespace Sphynx::Graphics::GL

