#pragma once
#include <DirectXMath.h>

namespace acc3d::ECS
{
    struct DirectionalLightComponent
    {
        alignas(16) DirectX::XMFLOAT3 Direction {0.0f,0.0f,0.0f}; // 4 * 3
    	alignas(16) DirectX::XMFLOAT3 Color{1.0f,1.0f,1.0f}; // 4 * 3
    	float Intensity = 0.0f; // 4 = 28
    };
}