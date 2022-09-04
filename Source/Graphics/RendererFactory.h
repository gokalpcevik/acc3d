#pragma once

#include "Renderer.h"
#include "../Core/Window.h"
#include <memory>
#include <DirectXMath.h>

namespace acc3d::Graphics
{
    class RendererFactory
    {
    public:
        RendererFactory() = default;

        static std::unique_ptr<Renderer> CreateRenderer(Core::Window const &window);
    };


}