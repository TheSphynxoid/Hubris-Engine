#include "pch.h"
#include "Engine.h"
#include "Core/Graphics/Window.h"
#ifdef SPH_WINDOWS
#include "Core/Graphics/Vulkan/vkBackend.h"
#include "Core/Graphics/Vulkan/vkWindow.h"
#endif

using namespace Sphynx::Graphics;
using namespace Sphynx;

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
		Vulkan::VulkanBackend::Init();
		break;
	case RenderAPI::None:
		//Special Headless mode. I haven't yet conceptualized it.
		break;
	default:
		return;
	}
}