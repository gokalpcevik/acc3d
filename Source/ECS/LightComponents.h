#pragma once
#include <DirectXMath.h>
#include "../Graphics/Type.h"

namespace acc3d::ECS
{
    struct DirectionalLightComponent
    {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT3 Direction;
        DirectX::XMFLOAT3 Color;
        float Intensity = 0.0f;

        Graphics::LightId Id = Graphics::LIGHT_ID_INVALID_VALUE;
    private:
    };



}