#pragma once

#include <cstdint>
#include <filesystem>
#include "../Graphics/Type.h"
#include "../Graphics/ShaderLibrary.h"
#include "../AssetCore/MeshLibrary.h"

namespace acc3d
{
	namespace Graphics
	{
		class Renderer;
        struct Drawable;
	}
}

namespace acc3d::ECS
{
    struct MeshRendererComponent
    {
        MeshRendererComponent(Asset::MeshAssetId id, Graphics::RootSignatureInitializer rootSignatureDescription)
    	: MeshAssetId(id), RootSignatureDescription(std::move(rootSignatureDescription)) {}

        Asset::MeshAssetId MeshAssetId = 0;

    	Graphics::RootSignatureInitializer RootSignatureDescription;

    	uint32_t Color = 0xFFFF00F0;
    private:
        Graphics::Drawable* Drawable = nullptr;

        friend class Graphics::Renderer;
    };

} //
