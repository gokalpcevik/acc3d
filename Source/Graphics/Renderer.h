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
#include "Drawable.h"
#include "ShaderReflection.h"
#include "../AssetCore/MeshLibrary.h"
#include "../Core/Window.h"
#include "../ECS/Scene.h"

#undef NOMINMAX

namespace acc3d::Graphics
{
    struct RendererPresentMethod
    {
        bool EnableVSync = true;
    };

    class Renderer
    {
    public:
        Renderer() = default;

        virtual ~Renderer();

        void Clear(FLOAT const *clearColor) const;

        void Present();

        void Resize(uint32_t width, uint32_t height);

        void ResizeDepthBuffer(uint32_t width, uint32_t height);

        void UpdateRenderTargetViews();

        void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE rtv,FLOAT const* clearColor) const;

        void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE dsv,D3D12_CLEAR_FLAGS flags = D3D12_CLEAR_FLAG_DEPTH,FLOAT depth = 1.0f) const;

        void RenderScene(ECS::Scene& scene);

        [[nodiscard]] std::unique_ptr<Device> const &GetDevice() const;

    	[[nodiscard]] RendererId GenerateRendererId();

        // This function is not ideal because every time this gets called, we have to perform a flush followed by a stall
        // for the copy operation of the buffers/resources. An ideal mesh renderer component register function would be
        // to possible combine several components into a range, record all the copy commands required for the creation of
        // the drawable object; and then wait until the copy command queue finishes.
        void RegisterMeshRendererComponentDrawable(RendererId rendererId, Asset::MeshAssetId meshAssetId);
        void DeregisterMeshRendererComponentDrawable(RendererId id);
    private:
        Core::Window const *m_Window{nullptr};

/*--------------PRESENT / CLEAR / RENDERING -----------------------*/
        std::unique_ptr<Device> m_Device;
        std::unique_ptr<SwapChain> m_SwapChain;
        std::unique_ptr<CommandQueue> m_DirectCmdQueue;
        std::unique_ptr<CommandQueue> m_CopyCmdQueue;
        std::unique_ptr<CommandList> m_GfxCmdList;
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
        std::unique_ptr<Fence> m_Fence;
        uint64_t m_FenceValue = 0;
        uint64_t m_FrameFenceValues[g_NUM_FRAMES_IN_FLIGHT] = {};
        HANDLE m_FenceEvent = INVALID_HANDLE_VALUE;

        std::unique_ptr<Fence> m_CopyFence;
        uint64_t m_CopyFenceValue = 0;
        HANDLE m_CopyFenceEvent = INVALID_HANDLE_VALUE;
/*-----------------------------------------------------------------*/

        RendererPresentMethod m_PresentMethod{true};

        google::dense_hash_map<RendererId, Drawable*> m_DrawableMap;
        // This should be atomically increased as we generate a renderer id, that is when we switch to a
        // multi-threaded renderer architecture
    	RendererId m_RendererIdValue = 2;

        friend class RendererFactory;
    };
}
