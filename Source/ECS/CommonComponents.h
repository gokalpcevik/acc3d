#pragma once

#include <DirectXMath.h>
#include <utility>
#include <string>

namespace acc3d::ECS
{
    struct TransformComponent
    {
        TransformComponent() = default;

        TransformComponent(DirectX::XMFLOAT3 translation, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale)
                : Translation(std::move(translation)), Rotation(std::move(rotation)),
                  Scale(std::move(scale))
        {}

        DirectX::XMFLOAT3 Translation{};
        DirectX::XMFLOAT3 Rotation{ 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3 Scale{};
    };

    struct TagComponent
    {
        TagComponent() = default;

        explicit TagComponent(std::string tag) : Tag(std::move(tag))
        {}

        std::string Tag;
    };
} // namespace Q3D
