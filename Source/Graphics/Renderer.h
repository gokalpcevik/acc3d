#pragma once
#define NOMINMAX

#include <memory>
#include <d3d12.h>
#include <D3D12MemAlloc.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <sparsehash/dense_hash_map>

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_sdl.h>

#include "Wrappers/Device.h"
#include "Wrappers/Fence.h"
#include "Wrappers/SwapChain.h"
#include "Wrappers/CommandQueue.h"
#include "Wrappers/CommandList.h"
#include "Wrappers/CommandAllocator.h"
#include "Wrappers/DescriptorHeap.h"
#include "Wrappers/RootSignature.h"
#include "Wrappers/PipelineState.h"
#include "Wrappers/Resource.h"
#include "Wrappers/InfoQueue.h"

#include "Memory/DescriptorAllocator.h"
#include "Synchronizer.h"
#include "ShaderLibrary.h"
#include "RootSignatureLibrary.h"
#include "Type.h"
#include "Drawable.h"
#include "DrawableFactory.h"
#include "LightContext.h"
#include "ShaderReflection.h"

#include "../AssetCore/MeshLibrary.h"
#include "../Core/Window.h"
#include "../ECS/Scene.h"

#undef NOMINMAX

namespace acc3d::Graphics
{
    struct PresentMethod
    {
        bool EnableVSync = true;
    };

    class Renderer
    {
    public:
        Renderer() = default;

        virtual ~Renderer() = default;

        void Clear(FLOAT const *clearColor) const;

        void Present();

        void Resize(uint32_t width, uint32_t height);

        void ResizeDepthBuffer(uint32_t width, uint32_t height);

        void UpdateRenderTargetViews();

        void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE rtv,FLOAT const* clearColor) const;

        void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE dsv,D3D12_CLEAR_FLAGS flags = D3D12_CLEAR_FLAG_DEPTH,FLOAT depth = 1.0f) const;

        void RenderScene(ECS::Scene& scene);

        void InitializeImGui();

        void RenderImGui() const;

        [[nodiscard]] size_t GetFrameDrawCallCount() const;

        [[nodiscard]] std::unique_ptr<Device> const &GetDevice() const;

        [[nodiscard]] D3D12MA::Stats GetAllocatorStats() const;

        [[nodiscard]] uint64_t GetFrameNumber() const { return m_FenceValue; }

        void RegisterMeshRendererComponentDrawable(ECS::MeshRendererComponent & mrc);

    	void DeregisterMeshRendererComponentDrawable(ECS::MeshRendererComponent& mrc);

        void Shutdown();

    private:

        void CreateImGuiFontDescriptorHeap();

    private:
        Core::Window const *m_Window{nullptr};

/*--------------PRESENT / CLEAR / RENDERING -----------------------*/
        std::unique_ptr<Device> m_Device;
        std::unique_ptr<SwapChain> m_SwapChain;
        std::unique_ptr<CommandQueue> m_DirectCmdQueue;
        std::unique_ptr<CommandQueue> m_CopyCmdQueue;
        std::unique_ptr<CommandList> m_GfxCmdList;
        std::unique_ptr<Resource> m_BackBuffers[g_NUM_FRAMES_IN_FLIGHT];
        D3D12MA::Allocation* m_DepthBuffer;
        std::unique_ptr<CommandAllocator> m_GfxCmdAllocators[g_NUM_FRAMES_IN_FLIGHT];
        std::unique_ptr<DescriptorHeap> m_RTVDescriptorHeap;
        std::unique_ptr<DescriptorHeap> m_DSVDescriptorHeap;
        DescriptorSizeInfo m_DescriptorHeapSizeInfo{};
        uint64_t m_CurrentBackBufferIndex = 0;

        D3D12_VIEWPORT m_Viewport{CD3DX12_VIEWPORT(0.0f,0.0f,1600,900)};
        D3D12_RECT m_ScissorRect{CD3DX12_RECT{0,0,LONG_MAX,LONG_MAX}};
/*#################################################################*/

/*------------------------------DEBUG------------------------------*/
#if defined(_DEBUG) || defined(DEBUG)
        std::unique_ptr<InfoQueue> m_InfoQueue;
#endif
/*#################################################################*/


/* --------------------SYNCHRONIZATION-----------------------------*/
        std::unique_ptr<Fence> m_Fence;
        uint64_t m_FenceValue = 0;
        uint64_t m_FrameFenceValues[g_NUM_FRAMES_IN_FLIGHT] = {};
        HANDLE m_FenceEvent = INVALID_HANDLE_VALUE;

        std::unique_ptr<Fence> m_CopyFence;
        uint64_t m_CopyFenceValue = 0;
        HANDLE m_CopyFenceEvent = INVALID_HANDLE_VALUE;
/*#################################################################*/

        PresentMethod m_PresentMethod{true};

        size_t mutable m_FrameDrawCallCount = 0;
/*-----------------------------------------------------------------*/

        std::unique_ptr<LightContext> m_LightContext;


/*---------------------MEMORY MANAGEMENT---------------------------*/
        D3D12MA::Allocator* m_Allocator = nullptr;

        std::unique_ptr<Memory::DescriptorAllocator> m_CBV_UAV_SRVDescriptorAllocator;
        std::unique_ptr<Memory::DescriptorAllocator> m_SamplerDescriptorAllocator;

        // Main GPU visible descriptor heaps. These will basically get set very few times throughout the scene
        // or the application.
        Memory::DescriptorAllocation m_MainDescriptorHeap;
        Memory::DescriptorAllocation m_SamplerDescriptorHeap;

/*#################################################################*/

/*-------------------------Dear ImGui------------------------------*/

        ID3D12DescriptorHeap* m_ImGuiFontDescriptorHeap = nullptr;

        Memory::DescriptorAllocation m_ImGuiFontAtlasDescriptorAllocation{};


/*#################################################################*/

        friend class RendererFactory;
    };
}
