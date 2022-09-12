#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include "Resource.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "Type.h"
#include "../AssetCore/MeshLibrary.h"

namespace acc3d::Graphics
{
    struct Drawable
    {
        Asset::MeshAssetId AssetId;
        RendererId RendererId = RENDERER_ID_EMPTY_KEY_VALUE;

        std::unique_ptr<Resource> VertexBuffer;
        std::unique_ptr<Resource> IndexBuffer;
        D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
        D3D12_INDEX_BUFFER_VIEW IndexBufferView;
        size_t IndicesCount = 0;

        /*
         *  •The root signature is the object that represents the link between the command list and the resources/constants
         * used by the pipeline.
         *
         *  •It specifies the data that the shaders should expect from the application; and also which pipeline state objects
         *  are compatible for the next draw/dispatch calls.
         *
         */

        std::unique_ptr<RootSignature> RootSignature;

        std::unique_ptr<PipelineState> PipelineState;
        /*
         * Pipeline state object is used to describe how the graphics/compute pipeline will behave
         * in every pipeline stage when we are going to render something.
         *
         * The pipeline state object will also incorporate the definition of a root signature, which is
         * used to define the resource types (and their details) during the execution of the pipeline(e.g
         * textures exposed to be sampled in pixel shader code).
         *
         *  ---------------Pipeline state object's components---------------------------------------------
         *
         *•Shader Byte Code: shader code is first compiled and then translated in binary blob data that will
         * serve as an input for the PSO, a process described in the next section.
         *
         *•Vertex Format Input Layout: definition of how vertex data is composed when passed to the Input assembler stage.
         * A definition in C++ code will correspond to a struct format in HLSL vertex shader code.
         *
         *•Primitive Topology Type: (point, line, triangle, patch) specifies how the input assembler needs to interpret vertex
         *data in order to generate 3D geometry along the pipeline.
         *
         *•Blend State: Blend state settings are used in the output merger, after the pixel shader, when we define how the just
         * rendered pixel should blend with the content already present in the render target we are writing into. This operation
         * happens just after the depth and stencil tests took place in the output merger.
         *
         *•Rasterizer State: Rasterizer state configures the fixed-pipeline stage that performs per-vertex transformation of data
         *in our geometry in normalized device coordinates and generates per-pixel data in screen-space, to be an input for the pixel
         *shader. This stage will always perform clipping operation, z-divide to provide perspective, viewport transformation and
         *depth-bias, all this before performing the actual rasterization.
         *
         *•Number of render targets and render target formats
         *
         *•Depth-stencil data format for the bound depth stencil buffer
         *
         *•Stream output buffer description
         *
         *•Root signature describes the set of resources (e.g. textures) available in each programmable stage.
         */
    };
}