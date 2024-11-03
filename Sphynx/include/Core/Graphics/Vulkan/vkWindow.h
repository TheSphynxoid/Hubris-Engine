#pragma once
#include <Core/Graphics/Window.h>

namespace Sphynx::Graphics::Vulkan {
	class vkWindow {
	public:
		vkWindow(int Width, int Height, std::string title);
		virtual ~vkWindow();
		virtual void Init();
		virtual void Render()noexcept = 0;
		virtual void Close()noexcept = 0;
		virtual Viewport GetViewport()const noexcept = 0;
		virtual bool IsValid()const noexcept = 0;
	};
}