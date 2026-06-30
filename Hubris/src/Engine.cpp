#include "pch.h"
#include "Engine.h"
#include "Core/Graphics/Window.h"
#ifdef HBR_WINDOWS
#include "Core/Graphics/Vulkan/vkBackend.h"
#include "Core/Graphics/Vulkan/vkWindow.h"
#endif

using namespace Hubris::Graphics;
using namespace Hubris;

void Engine::InitGraphics(const EngineConfig & config)
{
	switch (config.GraphicsBackend)
	{
	case RenderAPI::DX11:
		[[fallthrough]];
	case RenderAPI::OpenGL:
		break;
	case RenderAPI::DX12:
		[[fallthrough]];
	case RenderAPI::Vulkan:
#ifdef HBR_APPLE
		static_assert(false, "MoltenVK is not available");
#endif
		// Backend setup only — no window. Init is headless; the client creates a window
		// explicitly via Engine::CreateWindow() after Init if one is wanted.
		Vulkan::VulkanWindow::InitGLFW();
		Vulkan::VulkanBackend::CreateInstance();
		break;
	case RenderAPI::None:
		//Special Headless mode. I haven't yet conceptualized it.
		break;
	default:
		return;
	}
}

void Engine::CreateWindowInternal(const WindowCreateInfo& info)
{
	// Backend dispatch by the configured RenderAPI. Currently Vulkan-only on Windows;
	// DX12 is a future target. When DX12 lands, thread the API choice through here
	// (it's currently captured in EngineConfig at Init but not stored on Engine).
#ifdef HBR_WINDOWS
	window = Vulkan::VulkanWindow::Create(info.width, info.height, info.title);
#endif
}