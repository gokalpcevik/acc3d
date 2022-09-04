#pragma once

#include <filesystem>
#include <unordered_map>
#include <tuple>
#include <optional>
#include "MeshImporter.h"
#include "../Core/Log.h"

namespace acc3d::AssetCore
{
    using MeshAssetId = size_t;

    namespace fls = std::filesystem;

    class MeshLibrary
    {
    public:
        MeshLibrary() = default;

        MeshLibrary(const MeshLibrary &) = delete;

        // USAGE NOTE: Check the imported mesh data vector sizes to confirm the import
        // succeeded.
        static std::tuple<MeshAssetId, MeshAssetContent> Load(const fls::path &path);

        static std::optional<MeshAssetContent> CheckedCopy(MeshAssetId id);

        static void Remove(MeshAssetId id);

        static bool IsLoaded(MeshAssetId id);

        static MeshAssetId GetIdFromPath(const fls::path &path);

        static MeshAssetContent const &Retrieve(MeshAssetId id);

        static MeshAssetContent &&Move(MeshAssetId id);

        static MeshAssetContent Copy(MeshAssetId id);

    private:
        static std::unordered_map<MeshAssetId, MeshAssetContent> s_MeshIdMap;
    };
} //
