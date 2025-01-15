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

	struct Version {
		int Variant, Major, Minor, Patch;
	};

	struct EngineConfig {
		/**
		 * @brief Maximum of Thread to be spawned.
		 */
		unsigned int ThreadCount = std::thread::hardware_concurrency();
		RenderAPI GraphicsBackend = RenderAPI::Vulkan;
		/**
		* @brief Set to empty string to make the execution folder the root.
		* 
		* Default Value is an Empty string.
		*/
		std::string FileRoot = "";
		/**
		 * @brief The Application/Game Name. Will be set as the Title of the window (used also by vulkan).
		 */
		std::string ProjectName = "Default";

		Version ProjectVersion = { 0,0,0,0 };

		Graphics::Viewport WindowDimension;
	};

	class GraphicsManager final {
		friend class Engine;

		static void SetAPI(const RenderAPI& api);
	public:
		static void CreateWindow();
	};

	class Engine {
	private:
		static inline bool Started = false;
		static inline std::string ProjectName;
		static inline Version ProjectVersion;
		static inline Graphics::Window* window;
		static void InitGraphics(const EngineConfig& config);
	public:
		static EngineConfig GetPlatformConfig(std::string ProjectName = "Default", const Version& ProjectVersion = {0,0,0,0}) {
			static EngineConfig config;
			static bool HasInit = false;
			if (!HasInit) {
				config.ThreadCount = std::thread::hardware_concurrency() - 1;
				if constexpr (Platform == Sph_Platform::Windows || Platform == Sph_Platform::Linux) {
					config.GraphicsBackend = RenderAPI::Vulkan;
				}
			}
			config.ProjectName = ProjectName;
			config.ProjectVersion = ProjectVersion;
			return config;
		}
		/**
		 * @brief Initializes the engine (headless) for the first time called.
		 */
		static void Init(const EngineConfig& config) noexcept {
			if (Started) {
				Logger::Log("Engine Already started.");
				return;
			}
			ProjectName = config.ProjectName;
			//ThreadPool::InitalizePool(config.ThreadCount);
			InitGraphics(config);
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

		static void CreateWindow() {}
		static Graphics::Window* GetWindow() {};
		static inline std::string GetProjectName() noexcept { return ProjectName; };
		static inline const Version& GetProjectVersion()noexcept { return ProjectVersion; };
	};
}
