#pragma once

#include <Eigen/Eigen>
#include <utility>
#include <string>

namespace acc3d::ECS
{
    using Eigen::Matrix4f;
    using Eigen::Vector4f;
    using Eigen::Vector3f;

    struct TransformComponent
    {
        TransformComponent() = default;

        TransformComponent(Vector3f translation, Vector3f rotation, Vector3f scale)
                : Translation(std::move(translation)), Rotation(std::move(rotation)),
                  Scale(std::move(scale))
        {}

        Vector3f Translation{};
        Vector3f Rotation{0.0f, 0.0f, 0.0f};
        Vector3f Scale{};
    };

    struct TagComponent
    {
        TagComponent() = default;

        explicit TagComponent(std::string tag) : Tag(std::move(tag))
        {}

        std::string Tag;
    };
} // namespace Q3D
