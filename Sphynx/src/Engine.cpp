#include "pch.h"
#include "Engine.h"

using namespace Sphynx::Graphics;
using namespace Sphynx;

Window* Engine::InitGraphics(const EngineConfig & config)
{
	switch (config.GraphicsBackend == Sphynx::RenderAPI::None ? config.FallbackBackend : config.GraphicsBackend)
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
		
		break;
	case RenderAPI::None:
		break;
	default:
		break;
	}
}