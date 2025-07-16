#include "pch.h"
#include "Engine.h"
#include "Core/Graphics/Window.h"
#ifdef SPH_WINDOWS
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
		//break;
		[[fallthrough]];
	case RenderAPI::OpenGL:
		break;
	case RenderAPI::DX12:
		/*break;*/
		[[fallthrough]];
	case RenderAPI::Vulkan:
#ifdef SPH_APPLE
		static_assert("MoltenVK is not available");
#endif
		// auto name = config.ProjectName;
		// Vulkan::vkWindow::Create(config.WindowDimension, name.data());
		Vulkan::vkWindow::InitGLFW();
		Vulkan::vkBackend::CreateInstance();
		//TODO: Init is supposed to be headless, i'm doing this here because i want to test everything in the graphics together.
		window = Vulkan::vkWindow::Create(config.WindowDimension.Width, config.WindowDimension.Height, config.ProjectName);
		break;
	case RenderAPI::None:
		//Special Headless mode. I haven't yet conceptualized it.
		break;
	default:
		return;
	}
}