#pragma once
#define NOMINMAX

#include <memory>
#include <chrono>
#include <fmt/format.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include "Device.h"
#include "Fence.h"
#include "SwapChain.h"
#include "CommandQueue.h"
#include "CommandList.h"
#include "CommandAllocator.h"
#include "DescriptorHeap.h"
#include "Synchronizer.h"
#include "../Core/Window.h"

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

        void Render(FLOAT const *clearColor);

        void Resize(uint32_t width, uint32_t height);

        void UpdateRenderTargetViews();

        [[nodiscard]] std::unique_ptr<Device> const &GetDevice() const;

        [[nodiscard]] std::unique_ptr<Fence> const &GetFence() const;

    private:
        Core::Window const *m_Window{nullptr};

        std::unique_ptr<Device> m_Device;
        std::unique_ptr<SwapChain> m_SwapChain;
        std::unique_ptr<CommandQueue> m_CmdQueue;
        std::unique_ptr<GraphicsCommandList> m_GfxCmdList;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_BackBuffers[g_NUM_FRAMES_IN_FLIGHT];
        std::unique_ptr<CommandAllocator> m_CmdAllocators[g_NUM_FRAMES_IN_FLIGHT];
        std::unique_ptr<DescriptorHeap> m_RTVDescriptorHeap;
        std::unique_ptr<DescriptorHeap> m_DSVDescriptorHeap;

        DescriptorHeapSizeInfo m_DescriptorHeapSizeInfo{};

        uint64_t m_CurrentBackBufferIndex = 0;

        std::unique_ptr<Fence> m_Fence;
        uint64_t m_FenceValue = 0;
        uint64_t m_FrameFenceValues[g_NUM_FRAMES_IN_FLIGHT] = {};
        HANDLE m_FenceEvent = INVALID_HANDLE_VALUE;

        RendererPresentMethod m_PresentMethod{};

        friend class RendererFactory;
    };
}
