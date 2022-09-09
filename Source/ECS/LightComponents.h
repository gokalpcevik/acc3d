#pragma once
#include <DirectXMath.h>
#include "../Graphics/Type.h"

namespace acc3d::ECS
{
    struct DirectionalLightComponent
    {
        DirectX::XMFLOAT3 Direction {0.0f,0.0f,0.0f}; // 4 * 3
    private:
    	float r_padding0{};
    public:
    	DirectX::XMFLOAT3 Color{1.0f,1.0f,1.0f}; // 4 * 3
    	float Intensity = 0.0f; // 4 = 28
    };
}