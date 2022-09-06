#pragma once

#include <cstdint>
#include "../Graphics/Type.h"
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
    class RendererIdAccessor;

    struct MeshRendererComponent
    {
        MeshRendererComponent() = default;
        Asset::MeshAssetId MeshAssetId = 0;
    	uint32_t Color = 0xFFFF00F0;

    private:
        Graphics::RendererId RendererId = 0;
        friend class RendererIdAccessor;
    };

    class RendererIdAccessor
    {
    private:
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
