#pragma once

#include <DirectXMath.h>
#include <utility>
#include <string>

namespace acc3d::ECS
{
    struct TransformComponent
    {
        TransformComponent() = default;

        TransformComponent(DirectX::XMFLOAT3 translation, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 scale)
            : Translation(std::move(translation)), Rotation(DirectX::XMLoadFloat4(&rotation)),
            Scale(std::move(scale))
        {
        }

        [[nodiscard]] DirectX::XMMATRIX GetTransformationMatrix() const noexcept
        {
            return DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z) *
                DirectX::XMMatrixRotationQuaternion(Rotation) *
                DirectX::XMMatrixTranslation(Translation.x, Translation.y, Translation.z);
        }

        DirectX::XMFLOAT3 Translation{0.0f, 0.0f, 0.0f};
        DirectX::XMVECTOR Rotation{ 1.0f,0.0f,0.0f,0.0f };
        DirectX::XMFLOAT3 Scale{1.0f,1.0f,1.0f};
    };

    struct TagComponent
    {
        TagComponent() = default;

        explicit TagComponent(std::string tag) : Tag(std::move(tag))
        {}

        std::string Tag;
    };
} // namespace Q3D
