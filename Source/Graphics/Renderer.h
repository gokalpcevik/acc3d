#pragma once
#define NOMINMAX

#include <memory>
#include <chrono>
#include <fmt/format.h>
#include <d3d12.h>
#include <fmt/xchar.h>
#include <d3dcompiler.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <sparsehash/dense_hash_map>

#include "Device.h"
#include "Fence.h"
#include "SwapChain.h"
#include "CommandQueue.h"
#include "CommandList.h"
#include "CommandAllocator.h"
#include "DescriptorHeap.h"
#include "Synchronizer.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "ShaderLibrary.h"
#include "Resource.h"
#include "InfoQueue.h"
#include "Type.h"
#include "../AssetCore/MeshLibrary.h"
#include "../Core/Window.h"
#include "../ECS/Scene.h"
#include "../Util/RandomGenerator.h"

#undef NOMINMAX

namespace acc3d::Graphics
{
    template<typename T>
    using Scope = std::unique_ptr<T>;

    struct Drawable
    {
        Asset::MeshAssetId AssetId;

        Scope<Resource> VertexBuffer;
        Scope<Resource> IndexBuffer;
        D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
        D3D12_INDEX_BUFFER_VIEW IndexBufferView;

/*
 *  •The root signature is the object that represents the link between the command list and the resources/constants
 * used by the pipeline.
 *
 *  •It specifies the data that the shaders should expect from the application; and also which pipeline state objects
 *  are compatible for the next draw/dispatch calls. 
 *
 */

        Scope<RootSignature> RootSignature;

        Scope<PipelineState> PipelineStateObject;
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

    struct MeshRenderInfo
    {
        std::unique_ptr<RootSignature> RootSignature;

        D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
        D3D12_INDEX_BUFFER_VIEW IndexBufferView;

        Microsoft::WRL::ComPtr<ID3D12Resource> VertexBuffer;
        Microsoft::WRL::ComPtr<ID3D12Resource> IndexBuffer;

        Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineState;

        float FOV;
        DirectX::XMMATRIX ModelMatrix;
        DirectX::XMMATRIX ViewMatrix;
        DirectX::XMMATRIX ProjectionMatrix;

        size_t indicesCount;
    };

    struct RendererPresentMethod
    {
        bool EnableVSync = true;
    };



    class Renderer
    {
    public:
        Renderer() = default;

        virtual ~Renderer();

        void Render(FLOAT const *clearColor);

        void Resize(uint32_t width, uint32_t height);

        void ResizeDepthBuffer(uint32_t width, uint32_t height);

        void UpdateRenderTargetViews();

        void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE rtv,FLOAT const* clearColor);

        void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE dsv,D3D12_CLEAR_FLAGS flags = D3D12_CLEAR_FLAG_DEPTH,FLOAT depth = 1.0f);

        void RenderScene(ECS::Scene& scene);

        [[nodiscard]] std::unique_ptr<Device> const &GetDevice() const;

        static RendererId GenerateRendererId();

        void LoadTestScene();

        void RegisterMeshRendererComponentDrawable(RendererId id, Asset::MeshAssetId meshAssetId);
        void DeregisterMeshRendererComponentDrawable(RendererId id);
    private:
        Core::Window const *m_Window{nullptr};

/*--------------PRESENT / CLEAR / RENDERING -------------*/
        std::unique_ptr<Device> m_Device;
        std::unique_ptr<SwapChain> m_SwapChain;
        std::unique_ptr<CommandQueue> m_DirectCmdQueue;
        std::unique_ptr<CommandQueue> m_CopyCmdQueue;
        std::unique_ptr<GraphicsCommandList> m_GfxCmdList;
        std::unique_ptr<Resource> m_BackBuffers[g_NUM_FRAMES_IN_FLIGHT];
        std::unique_ptr<Resource> m_DepthBuffer;
        std::unique_ptr<CommandAllocator> m_GfxCmdAllocators[g_NUM_FRAMES_IN_FLIGHT];
        std::unique_ptr<DescriptorHeap> m_RTVDescriptorHeap;
        std::unique_ptr<DescriptorHeap> m_DSVDescriptorHeap;
        DescriptorSizeInfo m_DescriptorHeapSizeInfo{};
        uint64_t m_CurrentBackBufferIndex = 0;

        D3D12_VIEWPORT m_Viewport{CD3DX12_VIEWPORT(0.0f,0.0f,1600,900)};
        D3D12_RECT m_ScissorRect{CD3DX12_RECT{0,0,LONG_MAX,LONG_MAX}};
/*-----------------------------------------------------------------*/



/*------------------------------DEBUG------------------------------*/
#if defined(_DEBUG) || defined(DEBUG)
        std::unique_ptr<InfoQueue> m_InfoQueue;
#endif
/*-----------------------------------------------------------------*/


/* --------------------SYNCHRONIZATION-----------------------------*/
        std::unique_ptr<Fence> m_DirectFence;
        uint64_t m_FenceValue = 0;
        uint64_t m_FrameFenceValues[g_NUM_FRAMES_IN_FLIGHT] = {};
        HANDLE m_DirectFenceEvent = INVALID_HANDLE_VALUE;
/*------------------------------------------------------------------*/

/*---------------------TEST CUBE DEMO SCENE VARIABLES---------------*/
        MeshRenderInfo m_CubeInfo;
/*------------------------------------------------------------------*/

        RendererPresentMethod m_PresentMethod{};

        google::dense_hash_map<RendererId, Drawable*> m_DrawableMap;

        friend class RendererFactory;
    };
}
