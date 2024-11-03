#pragma once
#include <Platform.h>
#include <Logger.h>
#include <Core/ThreadPool.h>
#include <Core/ThreaddingServer.h>
#include <Core/Graphics/Window.h>

namespace Sphynx {

	enum class RenderAPI : unsigned short {
		OpenGL, Vulkan, DX12, DX11, None
	};

	struct EngineConfig {
		/**
		 * @brief Maximum of Thread to be spawned.
		 */
		unsigned int ThreadCount = std::thread::hardware_concurrency();
		RenderAPI GraphicsBackend = RenderAPI::OpenGL;
		/**
		 * @brief In case the Demanded graphics backend is unavailable, fallback to this.
		 */
		RenderAPI FallbackBackend = RenderAPI::None;
		/**
		 * @brief The Application/Game Name. Will be set as the Title of the window (used also by vulkan).
		 */
		std::string ApplicationName;
		/**
		* @brief Set to empty string to make the execution folder the root.
		* 
		* Default Value is an Empty string.
		*/
		std::string FileRoot = "";


		Graphics::Viewport WindowDimension;
	};

	class Engine {
	private:
		static inline bool Started = false;
		static Graphics::Window* window;
		static Graphics::Window* InitGraphics(const EngineConfig& config);
	public:
		static EngineConfig GetPlatformConfig() {
			static EngineConfig config;
			static bool HasInit = false;
			if (!HasInit) {
				config.ThreadCount = std::thread::hardware_concurrency() - 1;
				if constexpr (Platform == Sph_Platform::Windows || Platform == Sph_Platform::Linux) {
					config.GraphicsBackend = RenderAPI::Vulkan;
					config.FallbackBackend = RenderAPI::OpenGL;
				}
			}
			return config;
		}
		/**
		 * @brief Initializes the engine for the first time called.
		 */
		static void Init(const EngineConfig& config) noexcept {
			if (Started) {
				Logger::Log("Engine Already started.");
				return;
			}

			ThreadPool::InitalizePool(config.ThreadCount);
			window = InitGraphics(config);
		}

		/**
		 * @brief Yields control to the engine logic. The Engine will run until the program exits. 
		 * 
		 * This Calls Loop() in a loop.
		 */
		static void Run() {

		}
		/**
		 * @brief The Engine Executes the main loop once, then returns control to user. 
		 */
		static void Loop() {

		}

		static void GetPluginManager() {

		}
	};
}
