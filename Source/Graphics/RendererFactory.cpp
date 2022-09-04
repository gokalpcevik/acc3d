#include "RendererFactory.h"

namespace acc3d::Graphics
{
    std::unique_ptr<Renderer> RendererFactory::CreateRenderer(Core::Window const &window)
    {
        auto pDevice = std::make_unique<Device>();
        if (!(pDevice->m_Device && pDevice->m_DXGIFactory))
        {
            acc3d_error("Aborting renderer creation! Device wasn't initialized properly.");
            return {nullptr};
        }
        auto pCmdQueue = std::make_unique<CommandQueue>(pDevice->GetD3D12Device(),
                                                        D3D12_COMMAND_LIST_TYPE_DIRECT);
        if (!pCmdQueue)
        {
            acc3d_error(
                    "Aborting renderer creation! Command queue  wasn't initialized properly.");
            return {nullptr};
        }

        auto pSwapChain = std::make_unique<SwapChain>(pDevice->GetDXGIFactory(),
                                                      pCmdQueue->GetCommandQueue(), window,
                                                      pDevice.get());
        if (!pSwapChain)
        {
            acc3d_error("Aborting renderer creation! SwapChain wasn't initialized properly.");
            return {nullptr};
        }
        auto pFence = std::make_unique<Fence>(pDevice->GetD3D12Device(), D3D12_FENCE_FLAG_NONE,
                                              0);
        DescriptorHeapSizeInfo descriptorHeapSizeInfo = DescriptorHeap::GetDescriptorHeapSizeInfo(
                pDevice->GetD3D12Device());

        auto &&[RTVDescriptorHeap, DSVDescriptorHeap] =
                DescriptorHeap::CreateDescriptorHeapsForSwapChainBuffers(
                        pDevice->GetD3D12Device());

        auto pRenderer = std::make_unique<Renderer>();
        pRenderer->m_Device = std::move(pDevice);
        pRenderer->m_SwapChain = std::move(pSwapChain);
        pRenderer->m_CmdQueue = std::move(pCmdQueue);
        pRenderer->m_Fence = std::move(pFence);
        pRenderer->m_DescriptorHeapSizeInfo = descriptorHeapSizeInfo;
        pRenderer->m_RTVDescriptorHeap = std::move(RTVDescriptorHeap);
        pRenderer->m_DSVDescriptorHeap = std::move(DSVDescriptorHeap);
        pRenderer->m_Window = &window;
        pRenderer->m_FenceEvent = Fence::CreateFenceEventHandle();

        for (auto &m_CmdAllocator: pRenderer->m_CmdAllocators)
        {
            m_CmdAllocator = std::make_unique<CommandAllocator>(
                    pRenderer->m_Device->GetD3D12Device(), D3D12_COMMAND_LIST_TYPE_DIRECT);
        }
        pRenderer->m_GfxCmdList = std::make_unique<GraphicsCommandList>(
                pRenderer->m_Device->GetD3D12Device(),
                pRenderer->m_CmdAllocators[pRenderer->m_CurrentBackBufferIndex]->GetCommandAllocator(),
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                nullptr);

        pRenderer->UpdateRenderTargetViews();
        return std::move(pRenderer);
    }
}
