#include "pch.h"
#include "IO/ResourceManager.h"
#include "Engine.h"                   // ResourceConfig full definition
#include "IO/VFS/VirtualFileSystem.h"
#include "IO/VFS/FileSystemLoader.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <climits>
#endif

namespace Hubris::IO {

    namespace {
        /// @brief Directory containing the running executable. CWD-independent so asset
        /// loading works regardless of where the user launches from.
        /// @todo Move to a shared Core/PathUtils.h once more callers need it.
        std::filesystem::path ExecutableDir() {
#ifdef _WIN32
            wchar_t buf[MAX_PATH];
            DWORD len = GetModuleFileNameW(nullptr, buf, MAX_PATH);
            if (len == 0) return std::filesystem::current_path();
            return std::filesystem::path(std::wstring(buf, len)).parent_path();
#else
            char buf[PATH_MAX];
            ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf));
            if (len <= 0) return std::filesystem::current_path();
            return std::filesystem::path(std::string(buf, len)).parent_path();
#endif
        }

        // Combine a type_index + path into a single cache key. FNV-style mixing; good
        // enough for a per-process cache (not persisted across runs — the runtime-shaped
        // .hpak hashing lives in the AssetPipeline).
        uint64_t CacheKey(std::type_index type, std::string_view path) {
            // Seed with the type_index hash so the same path can serve different types.
            uint64_t h = std::hash<std::type_index>{}(type);
            // FNV-1a over the path.
            constexpr uint64_t FNV_OFFSET = 14695981039346656037ULL;
            constexpr uint64_t FNV_PRIME  = 1099511628211ULL;
            uint64_t ph = FNV_OFFSET;
            for (char c : path) { ph ^= static_cast<uint64_t>(c); ph *= FNV_PRIME; }
            h ^= ph + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
            return h;
        }
    }

    ResourceManager::~ResourceManager() {
        ClearCache();
    }

    void ResourceManager::Initialize(const ResourceConfig& config) {
        auto& vfs = VFS::VFS();
        const std::filesystem::path exeDir = ExecutableDir();

        // Mount each spec from the config, exe-relative. The engine executes policy;
        // it does not define it. Clients own the mount list via EngineConfig.resources.
        for (const MountSpec& spec : config.mounts) {
            auto loader = Handle<VFS::FileSystemLoader>(
                exeDir / spec.relativePath,
                spec.mountPoint,
                spec.priority
            );
            vfs.Mount(std::move(loader));
        }
    }

    void* ResourceManager::Load(std::type_index type, std::string_view path) {
        const uint64_t key = CacheKey(type, path);

        // Fast path: already cached.
        {
            std::lock_guard lock(m_mutex);
            if (auto it = m_cache.find(key); it != m_cache.end()) {
                return it->second.obj;
            }
        }

        // Find the registered factory for this type.
        FactoryEntry factoryEntry;
        {
            std::lock_guard lock(m_mutex);
            auto it = m_factories.find(type);
            if (it == m_factories.end()) {
                Logger::Log("ResourceManager: no factory registered for requested type on '{}'", path);
                return nullptr;
            }
            factoryEntry = it->second;
        }

        // Fetch raw bytes via VFS (transient — freed below regardless of outcome).
        auto& vfs = VFS::VFS();
        auto rawHandle = vfs.Load(path);
        if (!rawHandle || rawHandle->Size() == 0) {
            Logger::Log("ResourceManager: VFS miss / empty for '{}'", path);
            return nullptr;
        }

        // Call the typed factory. Bytes are copied into the GPU resource here; after
        // this returns the raw buffer is no longer needed.
        void* obj = factoryEntry.factory(rawHandle->data, path);

        // Free the raw bytes via VFS immediately (transient ownership — this is the
        // fix for the aliasing footgun: bytes never persist past the factory call).
        vfs.Unload(*rawHandle);

        if (!obj) {
            Logger::Log("ResourceManager: factory returned null for '{}'", path);
            return nullptr;
        }

        // Insert into cache under the lock.
        {
            std::lock_guard lock(m_mutex);
            // Re-check: another thread may have loaded the same asset concurrently.
            if (auto it = m_cache.find(key); it != m_cache.end()) {
                // Race lost — discard our duplicate, keep the cached one.
                factoryEntry.destructor(obj);
                return it->second.obj;
            }
            m_cache.emplace(key, CacheEntry{ obj, factoryEntry.destructor, type });
        }

        return obj;
    }

    std::optional<VFS::AssetHandle> ResourceManager::LoadRaw(std::string_view path) {
        // No factory, no cache — straight pass-through to VFS. Caller owns the bytes.
        return VFS::VFS().Load(path);
    }

    void ResourceManager::Unload(std::string_view path) {
        // We don't know the type from path alone; scan for matching path-derived keys.
        // This is O(n) but Unload-by-path is expected to be rare (the cache is the
        // canonical store; hot-reload uses ClearCache + re-load). If this becomes hot,
        // maintain a path -> key index.
        std::lock_guard lock(m_mutex);
        for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
            // CacheKey combines type + path; without the type we can't recompute the
            // exact key. Stored entries carry type, but not path. For now, Unload(path)
            // is supported only via ClearCache() — leaving a clean no-op here with a
            // logged warning rather than a partial/buggy sweep.
            // (Phase 2 follow-up: store path in CacheEntry to enable targeted unload.)
        }
        Logger::Log("ResourceManager::Unload(path) is not yet implemented; use ClearCache().");
    }

    void ResourceManager::ClearCache() {
        std::lock_guard lock(m_mutex);
        for (auto& [key, entry] : m_cache) {
            if (entry.obj && entry.destructor) {
                entry.destructor(entry.obj);
            }
        }
        m_cache.clear();
    }

    size_t ResourceManager::CachedCount() const {
        std::lock_guard lock(m_mutex);
        return m_cache.size();
    }

} // namespace Hubris::IO
