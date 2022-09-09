#pragma once
#include <DirectXMath.h>
#include "../Graphics/Type.h"

namespace acc3d::ECS
{
    struct alignas(16) DirectionalLightComponent
    {
        DirectX::XMFLOAT3 Direction {0.0f,0.0f,0.0f}; // 4 * 3
        DirectX::XMFLOAT3 Color{1.0f,1.0f,1.0f}; // 4 * 3
    private:
        uint8_t r_padding[4] = {};
    public:
    	float Intensity = 0.0f; // 4 = 28

    private:
    };
}