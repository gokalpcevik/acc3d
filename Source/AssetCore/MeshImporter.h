#pragma once

#include <filesystem>
#include <vector>
#include <Eigen/Eigen>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../Graphics/Type.h"
#include "../Core/Log.h"

namespace acc3d::AssetCore
    {
        struct MeshAssetContent
        {
            std::vector<Graphics::Vertex> Vertices{};
            std::vector<Graphics::Face> Faces{};
            std::vector<uint32_t> Indices{};
        };

        class MeshImporter
        {
        public:
            explicit MeshImporter(std::filesystem::path path);

            [[nodiscard]] bool ImportSucceeded() const;

            MeshAssetContent operator*() const
            {
                return m_Data;
            }

        private:
            std::filesystem::path m_Path{};
            MeshAssetContent m_Data{};
            bool m_Success = true;
        };
    }