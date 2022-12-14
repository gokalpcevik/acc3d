#include "MeshImporter.h"

namespace acc3d::Asset
{

    MeshImporter::MeshImporter(std::filesystem::path path)
            : m_Path(std::move(path))
    {
        this->Import(path, 0);
    }

    MeshImporter::MeshImporter(std::filesystem::path path, size_t meshIndex)
    {
        this->Import(path, meshIndex);
    }

    bool MeshImporter::ImportSucceeded() const
    {
        return m_Success;
    }

    void MeshImporter::Import(std::filesystem::path path, size_t meshIndex)
    {
        Assimp::Importer importer;
        auto const* pScene = importer.ReadFile(m_Path.string().c_str(), aiProcess_Triangulate |
            aiProcess_ConvertToLeftHanded);

        if (!pScene)
        {
            acc3d_error("Error while importing mesh: {0}", importer.GetErrorString());
            m_Success = false;
            return;
        }

        auto const* pMesh = pScene->mMeshes[meshIndex];
        m_Data.Vertices.reserve(pMesh->mNumVertices);
        m_Data.Indices.reserve(pMesh->mNumFaces * 3);

        for (size_t i = 0; i < pMesh->mNumVertices; ++i)
        {
            const Eigen::Vector3f pos = { pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z };
            const Eigen::Vector3f normal = { pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z };
            m_Data.Vertices.emplace_back(Graphics::Vertex{ pos, normal });
        }

        for (size_t i = 0; i < pMesh->mNumFaces; ++i)
        {
            auto const& face = pMesh->mFaces[i];
            m_Data.Indices.push_back(face.mIndices[0]);
            m_Data.Indices.push_back(face.mIndices[1]);
            m_Data.Indices.push_back(face.mIndices[2]);
        }
    }
}
