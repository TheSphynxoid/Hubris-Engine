#pragma once
#include <Platform.h>
#include <Logger.h>
#include <Core/ThreadPool.h>
#include <Core/ThreaddingServer.h>
#include <Core/Graphics/Window.h>
#include <Memory.h>
#include <Core/EventBus.h>

/// @brief The Hubris Engine main namespace.
namespace Hubris {


	struct Version {
		int Variant, Major, Minor, Patch;
	};
	/**
	 * \defgroup EngineGroup Engine Configuration and Management
	 * This group contains the Engine and its configuration details.
	 * @{
	 */
	enum class RenderAPI : unsigned short {
		OpenGL, Vulkan, DX12, DX11, None
	};

	typedef void(*StartupCallback)();
	/// @brief Used to configure the engine on instantiation.
	struct EngineConfig {
		/**
		 * @brief Maximum of Thread to be spawned. (default: std::thread::hardware_concurrency() - 1)
		 */
		unsigned int ThreadCount = std::thread::hardware_concurrency() - 1;
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

		Graphics::Viewport WindowDimension = {0, 0};

		StartupCallback StartUpCallback = nullptr;
	};
	/// @deprecated Here for library architure experiments, Strong possibility of removal.
	class GraphicsManager final {
		friend class Engine;

		static void SetAPI(const RenderAPI& api);
	public:
		static void CreateWindow();
	};
	/// @brief Stores and defines %Engine wide data and functionalities.
	class Engine final {
	private:
		static inline bool Started = false;
		static inline std::string ProjectName;
		static inline Version ProjectVersion;
		static inline Graphics::Window* window;
		static inline std::terminate_handler originalHandler = nullptr;
		static inline std::vector<const char*> Env = std::vector<const char*>(0);
		static void InitGraphics(const EngineConfig& config);


		static void Terminate() noexcept{
			Logger::Fatal("Engine Execution has been terminated.");
			Logger::Close();
			
			if(originalHandler){
				originalHandler();
			}
		}

		/// @brief The Engine class is purely static.
		Engine() = delete;
		~Engine() = delete;

	public:
		/**
		 * @brief Returns the default platform's (Windows, Linux, etc...) default engine configuration.
		 */
		static EngineConfig GetPlatformConfig(std::string ProjectName = "Default", const Version& ProjectVersion = {0,0,0,0}) noexcept {
			static EngineConfig config;
			static bool HasInit = false;
			if (!HasInit) {
				config.ThreadCount = std::thread::hardware_concurrency() - 1;
				if constexpr (Platform == Hbr_Platform::Windows || Platform == Hbr_Platform::Linux) {
					config.GraphicsBackend = RenderAPI::Vulkan;
				}
			}
			config.ProjectName = ProjectName;
			config.ProjectVersion = ProjectVersion;
			config.WindowDimension = {800, 600};

			return config;
		}
		/**
		 * @brief Initializes the engine (headless) for the first time called. TODO: Make sure this is a headless initializer.
		 */
		static void Init(const EngineConfig& config) noexcept {
			if (Started) {
				Logger::Log("Engine Already started.");
				return;
			}
			ProjectName = config.ProjectName;
			//ThreadPool::InitalizePool(config.ThreadCount);
			InitGraphics(config);

			originalHandler = std::set_terminate(Engine::Terminate);

			// if (config.StartUpCallback) {
			// 	config.StartUpCallback();
			// }

			Core::StaticEventBus<Core::OnStart>::Dispatch(Core::OnStart());
		}

		/**
		 * @brief Yields control to the engine logic. The Engine will run until the program exits. 
		 * 
		 * This Calls Loop() in a loop.
		 * 
		 * @warning This requires the Engine to be initialized first.
		 */
		static void Run() {
			while (window->IsRunning()) {
				Loop();
				Core::StaticEventBus<Core::OnUpdate>::Dispatch(Core::OnUpdate());
			}
		}
		/**
		 * @brief The Engine Executes the main loop once, then returns control to user. 
		 * 
		 * @warning This requires the Engine to be initialized first.
		 */
		static void Loop() {
			window->Update();
// #pragma warning (push) 
// #pragma warning (disable: 4996)
// 			_sleep(100);
// #pragma warning (pop)
		}

		static void Shutdown(){
			window->Close();
			//TODO: Add Grahpics cleanup, this needs some work.
			// GraphicsManager::Cleanup()
		}
		/**
		 * @brief Get the Plugin Manager object. [A planned feature]
		 * 
		 * @addindex NotImplemented
		 */
		static void GetPluginManager() {

		}

		static void CreateWindow() {}
		
		static Graphics::Window* GetWindow() {};
		static inline const std::string& GetProjectName() noexcept { return ProjectName; };
		static inline const Version& GetProjectVersion() noexcept { return ProjectVersion; };
		// static const char** GetVkRequiredExtensions() noexcept;
	};
	
	/** @} */ // End of EngineGroup
}
