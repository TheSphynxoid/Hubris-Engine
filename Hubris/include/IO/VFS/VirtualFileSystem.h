#pragma once
#include "IAssetLoader.h"
#include <vector>
#include <unordered_map>
#include <mutex>
#include <memory>

namespace Hubris::IO::VFS {

    /**
     * @brief Central Virtual Filesystem coordinator
     * Manages multiple asset loaders with priority-based resolution
     */
    class VirtualFileSystem {
    public:
        static VirtualFileSystem& Instance() {
            static VirtualFileSystem instance;
            return instance;
        }

        VirtualFileSystem(const VirtualFileSystem&) = delete;
        VirtualFileSystem& operator=(const VirtualFileSystem&) = delete;

        /**
         * @brief Mount a loader with specific priority
         * Higher priority loaders are checked first
         * @param loader Unique pointer to loader implementation
         * @param priority Mount priority (higher = checked first)
         */
        void Mount(Handle<IAssetLoader> loader, int priority = 0);

        /**
         * @brief Unmount a loader by name
         * @param loaderName Name returned by loader->Name()
         */
        void Unmount(std::string_view loaderName);

        /**
         * @brief Load asset by virtual path
         * Tries each loader in priority order until one succeeds
         * @param virtualPath Path like "shaders://vertex.spv"
         * @return AssetHandle if found, nullopt otherwise
         */
        [[nodiscard]] std::optional<AssetHandle> Load(std::string_view virtualPath);

        /**
         * @brief Unload an asset
         * Automatically routes to the correct loader
         * @param handle Handle to unload
         */
        void Unload(AssetHandle& handle);

        /**
         * @brief Check if asset exists without loading
         * @param virtualPath Path to check
         */
        [[nodiscard]] bool Exists(std::string_view virtualPath) const;

        /**
         * @brief Get list of mounted loaders (for debugging)
         */
        [[nodiscard]] std::vector<std::string> GetMountedLoaders() const;

        /**
         * @brief Clear all cached assets (hot-reload support)
         */
        void ClearCache();

    private:
        VirtualFileSystem() = default;
        ~VirtualFileSystem() = default;

        struct LoaderEntry {
            Handle<IAssetLoader> loader;
            int priority;
        };

        std::vector<LoaderEntry> m_loaders;
        std::unordered_map<uint64_t, size_t> m_handleToLoader; ///< Maps handle ID to loader index
        std::unordered_map<uint64_t, AssetHandle> m_cache;     ///< Asset cache
        mutable std::mutex m_mutex;

        void SortLoadersByPriority();
    };

    // Convenience alias
    inline VirtualFileSystem& VFS() { return VirtualFileSystem::Instance(); }

} // namespace Hubris::IO::VFS
