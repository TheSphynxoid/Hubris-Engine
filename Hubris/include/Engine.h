#pragma once
#include <Platform.h>
#include <Logger.h>
#include <Core/ThreadPool.h>
#include <Core/ThreaddingServer.h>
#include <Core/Graphics/Window.h>
#include <Memory.h>
#include <Core/EventBus.h>
#include <IO/ResourceManager.h>

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

namespace IO {
	/// @brief A VFS mount specification. The client declares what to mount; the engine
	/// provides the mechanism (ResourceManager mounts what it's told, exe-relative).
	struct MountSpec {
		std::string mountPoint;              ///< e.g. "shaders://"
		std::filesystem::path relativePath; ///< e.g. "assets/shaders" (relative to exe dir)
		int priority = 100;                  ///< Higher = queried first by VFS.
	};

	/// @brief Resource-layer policy carried by EngineConfig. Separates client intent
	/// (what to mount) from engine mechanism (how to mount). Factories are registered
	/// separately via explicit RegisterFactory<T> calls — config is about *storage*,
	/// factories are about *types*. The engine defines no default mounts; clients own
	/// their mount list entirely.
	struct ResourceConfig {
		std::vector<MountSpec> mounts;
	};
}

/// @brief Configuration for engine window creation. Vk-style info struct: plain fields,
/// default-initialized, passed by const-ref to Engine::CreateWindow. Extensible (fullscreen,
/// resizable, vsync) without breaking existing call sites.
struct WindowCreateInfo {
	uint32_t width = 800;
	uint32_t height = 600;
	std::string title = "Hubris";

	/// @brief What happens when the user closes the window (X-button, Alt+F4).
	enum class ClosePolicy : uint8_t {
		StopEngine,    ///< Engine's main loop exits. Standard desktop behavior (default).
		DetachWindow,  ///< Window closes/destroys; engine continues running headless.
	};
	ClosePolicy closePolicy = ClosePolicy::StopEngine;
};

/// @
// typedef void(*StartupCallback)();
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

		/// @brief Resource-layer policy: VFS mounts. Defaults to empty (engine presumes
		/// nothing); clients typically assign IO::ResourceConfig::Defaults() or build their own.
		IO::ResourceConfig resources;

		// StartupCallback StartUpCallback = nullptr;
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
		static inline bool running = false;  ///< Engine owns the loop condition; window is optional.
		static inline std::string ProjectName;
		static inline Version ProjectVersion;
		static inline Graphics::Window* window = nullptr;
		static inline WindowCreateInfo::ClosePolicy windowClosePolicy = WindowCreateInfo::ClosePolicy::StopEngine;
		static inline std::terminate_handler originalHandler = nullptr;
		static inline std::vector<const char*> Env = std::vector<const char*>(0);
		static void InitGraphics(const EngineConfig& config);
		static void CreateWindowInternal(const WindowCreateInfo& info);  ///< Backend-specific creation.


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
		 * @brief Initializes the engine (headless). No window is created here; call
		 * CreateWindow() after Init if a window is wanted. Truly headless: runs without
		 * any window, supports windowless/headless execution paths.
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

			// Resource layer bootstrap: mount what the client declared in config.resources,
			// exe-relative (CWD-independent). The engine executes policy; it does not define
			// it — no presumption of shaders:// or any asset kind. The client registers typed
			// factories explicitly via RegisterFactory<T> before/after Init.
			IO::ResourceManager::Instance().Initialize(config.resources);

			Started = true;
			running = true;

			// OnStart is NOT dispatched here — Init is headless. OnStart fires at the start
			// of Run(), so the client can create a window (CreateWindow) between Init and
			// Run and have it available to OnStart handlers.
		}

		/**
		 * @brief Runs the engine main loop. Loops on the engine's own `running` flag; the
		 * window is an optional service. If a window exists, polls its events once per
		 * iteration (non-blocking) and honors its close policy (X-button either stops the
		 * engine or detaches the window). Headless: no window, just loops on `running`.
		 * Dispatches OnStart once before the loop begins.
		 *
		 * @warning Requires the Engine to be initialized first.
		 */
		static void Run() {
			Core::StaticEventBus<Core::OnStart>::Dispatch(Core::OnStart());
			while (running) {
				if (window) {
					window->Update();  // one PollEvents, returns immediately
					if (!window->IsRunning()) {
						// User requested window close (X-button, Alt+F4).
						if (windowClosePolicy == WindowCreateInfo::ClosePolicy::StopEngine) {
							Shutdown();
							continue;
						} else {
							// Detach: destroy the window, keep running headless.
							delete window;
							window = nullptr;
						}
					}
				}
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
			// Per-frame work hooks go here (sim, render dispatch via systems, etc.).
		}

		/**
		 * @brief Stops the engine. Sets `running = false` (the Run loop exits on its next
		 * iteration) and closes the window if one exists. Safe to call headless (no window).
		 */
		static void Shutdown(){
			running = false;
			if (window) {
				window->Close();
			}
			//TODO: Add Graphics cleanup, this needs some work.
			// GraphicsManager::Cleanup()
		}
		/**
		 * @brief Get the Plugin Manager object. [A planned feature]
		 * 
		 * @addindex NotImplemented
		 */
		static void GetPluginManager() {

		}

		/// @brief Explicitly create the engine window. Optional — the engine runs headless
		/// without calling this. Stores the window and its close policy; Run() will poll
		/// the window's events each iteration and honor the close policy on X-button.
		/// @warning Must be called after Init(). Replaces any existing window.
		static void CreateWindow(const WindowCreateInfo& info = {}) {
			if (window) {
				delete window;
				window = nullptr;
			}
			windowClosePolicy = info.closePolicy;
			CreateWindowInternal(info);
		}
		
		static Graphics::Window* GetWindow() noexcept { return window; };
		static inline const std::string& GetProjectName() noexcept { return ProjectName; };
		static inline const Version& GetProjectVersion() noexcept { return ProjectVersion; };
		// static const char** GetVkRequiredExtensions() noexcept;
	};
	/** @} */ // End of EngineGroup
}
