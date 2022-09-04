#pragma once

#include <cstdint>
#include "../Graphics/Type.h"
#include "../AssetCore/MeshLibrary.h"

namespace acc3d::ECS
{
    struct MeshComponent
    {
        AssetCore::MeshAssetId MeshAssetId = 0;
        Graphics::RendererId RendererId = 0;
    	uint32_t Color = 0xFFFF00F0;
    };
} //
