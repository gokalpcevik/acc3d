#include "RendererFactory.h"

namespace acc3d::Graphics
{
    std::unique_ptr<Renderer> RendererFactory::CreateRenderer(Core::Window const &window)
    {
        if(!DirectX::XMVerifyCPUSupport())
        {
            acc3d_error("Aborting renderer creation! DirectX Math Library is not supported on "
                        "this system.");
            return {nullptr};
        }

        auto pDevice = std::make_unique<Device>();
        if (!(pDevice->m_Device && pDevice->m_DXGIFactory))
        {
            acc3d_error("Aborting renderer creation! Device wasn't initialized properly.");
            return {nullptr};
        }

        D3D12MA::ALLOCATOR_DESC allocatorDescription = {};
        allocatorDescription.pDevice = pDevice->GetD3D12DevicePtr();
        allocatorDescription.pAdapter = pDevice->GetDXGIAdapterPtr();

        D3D12MA::Allocator* pAllocator = nullptr;
        HRESULT hr = D3D12MA::CreateAllocator(&allocatorDescription, &pAllocator);

        if(FAILED(hr))
        {
            acc3d_error("D3D12MA::Allocator creation failed.");
            return { nullptr };
        }

        auto pCmdQueue = std::make_unique<CommandQueue>(pDevice->GetD3D12DevicePtr(),
                                                        D3D12_COMMAND_LIST_TYPE_DIRECT);

        auto pCopyCmdQueue = std::make_unique<CommandQueue>(pDevice->GetD3D12DevicePtr(),
            D3D12_COMMAND_LIST_TYPE_COPY);
        if (!(pCmdQueue && pCopyCmdQueue))
        {
            acc3d_error(
                    "Aborting renderer creation! Command queue  wasn't initialized properly.");
            return {nullptr};
        }

        auto pSwapChain = std::make_unique<SwapChain>(pDevice->GetDXGIFactoryPtr(),
                                                      pCmdQueue->m_CmdQueue.Get(), window,
                                                      pDevice.get());
        if (!pSwapChain)
        {
            acc3d_error("Aborting renderer creation! SwapChain wasn't initialized properly.");
            return {nullptr};
        }
        auto pDirectFence = std::make_unique<Fence>(pDevice->GetD3D12DevicePtr(), D3D12_FENCE_FLAG_NONE,
                                              0);
        auto pCopyFence = std::make_unique<Fence>(pDevice->GetD3D12DevicePtr(), D3D12_FENCE_FLAG_NONE,
            0);
        DescriptorSizeInfo const descriptorHeapSizeInfo = DescriptorHeap::GetDescriptorSizeInfo(
                pDevice->GetD3D12DevicePtr());

        auto &&[RTVDescriptorHeap, DSVDescriptorHeap] =
                DescriptorHeap::CreateDescriptorHeapsForSwapChainBuffers(
                        pDevice->GetD3D12DevicePtr()) ;

        auto pLightContext = std::make_unique<LightContext>();
        pLightContext->Populate(pDevice->GetD3D12DevicePtr(),pAllocator);

        auto pRenderer = std::make_unique<Renderer>();

        pRenderer->m_Device = std::move(pDevice);
        pRenderer->m_SwapChain = std::move(pSwapChain);
        pRenderer->m_Allocator = pAllocator;
    	pRenderer->m_DirectCmdQueue = std::move(pCmdQueue);
        pRenderer->m_CopyCmdQueue = std::move(pCopyCmdQueue);
        pRenderer->m_Fence = std::move(pDirectFence);
        pRenderer->m_CopyFence = std::move(pCopyFence);
        pRenderer->m_CopyFenceEvent = Fence::CreateFenceEventHandle();
    	pRenderer->m_DescriptorHeapSizeInfo = descriptorHeapSizeInfo;
        pRenderer->m_RTVDescriptorHeap = std::move(RTVDescriptorHeap);
        pRenderer->m_DSVDescriptorHeap = std::move(DSVDescriptorHeap);
    	pRenderer->m_Window = &window;
        pRenderer->m_FenceEvent = Fence::CreateFenceEventHandle();
        pRenderer->m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, window.GetSurfaceWidth(), window.GetSurfaceHeight(), 0.0f, 1.0f);
        pRenderer->m_LightContext = std::move(pLightContext);

#if defined(_DEBUG) || defined(DEBUG)
        auto pInfoQueue = std::make_unique<InfoQueue>(pRenderer->GetDevice()->GetD3D12DevicePtr());
        pRenderer->m_InfoQueue = std::move(pInfoQueue);
#endif

        for (auto &m_CmdAllocator: pRenderer->m_GfxCmdAllocators)
        {
            m_CmdAllocator = std::make_unique<CommandAllocator>(
                    pRenderer->m_Device->GetD3D12DevicePtr(), D3D12_COMMAND_LIST_TYPE_DIRECT);
        }
        pRenderer->m_GfxCmdList = std::make_unique<CommandList>(
                pRenderer->m_Device->GetD3D12DevicePtr(),
                pRenderer->m_GfxCmdAllocators[pRenderer->m_CurrentBackBufferIndex]->GetD3D12CommandAllocatorPtr(),
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                nullptr);

        pRenderer->InitDrawableDenseHashMap();

        ShaderLibrary::Init();
        RootSignatureLibrary::Init();

        pRenderer->UpdateRenderTargetViews();
        pRenderer->ResizeDepthBuffer(window.GetSurfaceWidth(),window.GetSurfaceHeight());

        pRenderer->InitializeImGui();
    	return std::move(pRenderer);
    }
}
