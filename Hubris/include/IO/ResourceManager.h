#pragma once
#include <filesystem>
#include <mutex>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "IO/VFS/AssetHandle.h"
#include "Memory.h"

namespace Hubris::IO {

    // Forward declaration: ResourceConfig is defined in Engine.h. We only need a
    // reference here, so the full definition isn't required at declaration time.
    struct ResourceConfig;

    /// @brief Typed asset cache + user-facing resource API.
    ///
    /// Two-component split with VFS: VFS is abstract storage (raw bytes from anywhere,
    /// priority overrides, DLC mounting); ResourceManager is the typed layer that owns
    /// parsed GPU resources and serves them by virtual path.
    ///
    /// @section Ownership
    /// Typed objects own their parsed GPU resources; raw bytes are transient.
    /// `Load<T>` fetches bytes via VFS, calls the registered typed factory, frees the
    /// bytes immediately, caches the `Handle<T>`. The cache therefore holds the canonical
    /// typed result; callers receive a non-owning `T*` view (RM is an engine-lifetime
    /// singleton, so the view outlives any caller). This dissolves the VFS
    /// "one Unload dangles everyone" aliasing bug — bytes are never held long-term.
    ///
    /// @section Reference model
    /// Dev-facing API is path-string (`Load<Shader>("shaders://foo")`); cache is keyed
    /// by hash of (type, path) internally. Preserves the option to cook path->hash at
    /// pack time (dev = readable, shipped = compact) without changing the API.
    ///
    /// @section Factory registration
    /// Explicit: `RegisterFactory<Shader>(factoryFn)` called from Engine init. Explicit
    /// over magic (static auto-register) because init-order races are hard to debug and
    /// the open-access philosophy favors explicit over implicit.
    class ResourceManager {
    public:
        /// Type-erased factory: takes raw asset bytes + the virtual path (for
        /// stage/metadata deduction, e.g. `_vert`/`_frag` suffix), returns an owning
        /// pointer to a heap-allocated typed object as `void*` for type-erased storage.
        /// The typed `Load<T>` knows the real type and casts back.
        /// @return Opaque pointer to the typed object, or nullptr on failure.
        using FactoryFn = void* (*)(std::span<const uint8_t> data, std::string_view path);

        /// @brief Per-type destructor (the cache stores `void*`, so it can't call `delete T`).
        using DestructorFn = void (*)(void* obj);

        static ResourceManager& Instance() {
            static ResourceManager instance;
            return instance;
        }

        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        /// @brief Register a typed factory. Called explicitly by the client (or a layer
        /// acting on the client's behalf, e.g. graphics). Not auto-invoked by the engine.
        template<typename T>
        void RegisterFactory(FactoryFn factory, DestructorFn destructor) {
            std::lock_guard lock(m_mutex);
            m_factories[std::type_index(typeid(T))] = { factory, destructor };
        }

        /// @brief Mount the VFS loaders declared in the config, exe-relative. Called by
        /// Engine::Init with the client-supplied ResourceConfig. The engine provides the
        /// mechanism (mount what it's told); the client provides the policy (what to mount
        /// via EngineConfig.resources). CWD-independent (exe-relative). No factory
        /// registration here — that's the client's explicit job via RegisterFactory<T>.
        void Initialize(const ResourceConfig& config);

        /// @brief Load a typed asset by virtual path. Returns a non-owning view; the
        /// cache owns the object for the engine's lifetime.
        /// @return Pointer to the cached typed object, or nullptr on failure.
        template<typename T>
        [[nodiscard]] T* Load(std::string_view path) {
            void* obj = Load(typeid(T), path);
            return static_cast<T*>(obj);
        }

        /// @brief Escape hatch: fetch raw bytes without a typed factory. Bytes are owned
        /// by the returned AssetHandle (caller must hold or copy). No caching.
        [[nodiscard]] std::optional<VFS::AssetHandle> LoadRaw(std::string_view path);

        /// @brief Drop a single cached asset (frees its memory via the registered destructor).
        void Unload(std::string_view path);

        /// @brief Clear the entire typed cache (frees all cached assets). Hot-reload hook.
        void ClearCache();

        /// @brief Number of cached assets (for debugging / profiling).
        [[nodiscard]] size_t CachedCount() const;

    private:
        ResourceManager() = default;
        ~ResourceManager();

        struct FactoryEntry {
            FactoryFn factory;
            DestructorFn destructor;
        };
        struct CacheEntry {
            void* obj;                 ///< Type-erased typed object (owned).
            DestructorFn destructor;   ///< How to destroy it.
            std::type_index type;
        };

        void* Load(std::type_index type, std::string_view path);

        mutable std::mutex m_mutex;
        std::unordered_map<std::type_index, FactoryEntry> m_factories;
        std::unordered_map<uint64_t, CacheEntry> m_cache;  ///< Keyed by hash(type, path)
    };

    /// @brief Convenience accessor.
    inline ResourceManager& Resources() { return ResourceManager::Instance(); }

} // namespace Hubris::IO
