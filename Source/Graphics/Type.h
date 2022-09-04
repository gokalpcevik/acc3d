#pragma once
#include <Eigen/Eigen>
#include <cstdint>

namespace acc3d::Graphics
{
    using RendererId = uint64_t;

    struct Face
    {
        Face() = default;
        // Indices
        uint32_t i0, i1, i2;
    };

    struct Vertex
    {
        Eigen::Vector4f Position{ 0.0f,0.0f,0.0f,1.0f };
        Eigen::Vector4f Normal{ 0.0f,0.0f,0.0f,0.0f };
    };
}