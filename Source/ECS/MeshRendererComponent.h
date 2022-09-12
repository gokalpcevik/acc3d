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
	}
}

namespace acc3d::ECS
{
    class RIDAccessor;

    struct MeshRendererComponent
    {
        MeshRendererComponent(Asset::MeshAssetId id, Graphics::RootSignatureInitializer rootSignatureDescription)
    	: MeshAssetId(id), RootSignatureDescription(std::move(rootSignatureDescription)) {}

        Asset::MeshAssetId MeshAssetId = 0;

    	Graphics::RootSignatureInitializer RootSignatureDescription;

    	uint32_t Color = 0xFFFF00F0;
    private:
        Graphics::RendererId RendererId = 0;
        friend class RIDAccessor;
    };

    class RIDAccessor
    {
	    Graphics::RendererId const& operator()(MeshRendererComponent const& mrc) const
        {
            return mrc.RendererId;
        }

        Graphics::RendererId& operator()(MeshRendererComponent& mrc) const
        {
            return mrc.RendererId;
        }

        friend class Scene;
        friend class Graphics::Renderer;
    };
} //
