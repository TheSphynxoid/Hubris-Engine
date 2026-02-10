#pragma once
#include "IAssetLoader.h"
#include <filesystem>
#include <fstream>
#include <vector>

namespace Hubris::IO::VFS {

    /**
     * @brief Loads assets from loose files on disk
     * Development-friendly loader for hot-reload workflows
     */
    class FileSystemLoader : public IAssetLoader {
    public:
        /**
         * @brief Construct filesystem loader
         * @param rootPath Physical root directory (e.g., "C:/GameAssets/")
         * @param mountPoint Virtual mount point (e.g., "assets://")
         * @param priority Loader priority
         */
        explicit FileSystemLoader(
            std::filesystem::path rootPath,
            std::string mountPoint = "assets://",
            int priority = 0
        );

        ~FileSystemLoader() override = default;

        [[nodiscard]] std::optional<AssetHandle> Load(std::string_view virtualPath) override;
        void Unload(AssetHandle& handle) override;
        [[nodiscard]] bool CanHandle(std::string_view virtualPath) const override;
        [[nodiscard]] int Priority() const noexcept override { return m_priority; }
        [[nodiscard]] std::string_view Name() const noexcept override { return "FileSystemLoader"; }

    private:
        std::filesystem::path m_rootPath;
        std::string m_mountPoint;
        int m_priority;

        [[nodiscard]] std::filesystem::path ResolvePhysicalPath(std::string_view virtualPath) const;
        [[nodiscard]] AssetType DeduceAssetType(const std::filesystem::path& path) const;
    };

} // namespace Hubris::IO::VFS
