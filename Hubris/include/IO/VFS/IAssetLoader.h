#pragma once
#include "AssetHandle.h"
#include <optional>
#include <string_view>
#include <memory>

namespace Hubris::IO::VFS {

    /**
     * @brief Abstract interface for asset loading backends
     * Implement this for different asset sources (filesystem, packages, memory)
     */
    class IAssetLoader {
    public:
        virtual ~IAssetLoader() = default;

        /**
         * @brief Load an asset by virtual path
         * @param virtualPath Path in VFS namespace (e.g., "shaders://vertex.spv")
         * @return AssetHandle if found, nullopt otherwise
         */
        [[nodiscard]] virtual std::optional<AssetHandle> Load(std::string_view virtualPath) = 0;

        /**
         * @brief Unload an asset and free resources
         * @param handle Handle returned by Load()
         */
        virtual void Unload(AssetHandle& handle) = 0;

        /**
         * @brief Check if this loader can handle the given path
         * @param virtualPath Path to check
         * @return true if this loader should attempt to load it
         */
        [[nodiscard]] virtual bool CanHandle(std::string_view virtualPath) const = 0;

        /**
         * @brief Get loader priority (higher = checked first)
         * Used for mount point ordering
         */
        [[nodiscard]] virtual int Priority() const noexcept = 0;

        /**
         * @brief Get loader name for debugging
         */
        [[nodiscard]] virtual std::string_view Name() const noexcept = 0;
    };

} // namespace Hubris::IO::VFS
