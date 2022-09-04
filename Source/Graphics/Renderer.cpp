#include "Renderer.h"

namespace acc3d::Graphics
{
	using Microsoft::WRL::ComPtr;

	DescriptorHeapSizeInfo Renderer::GetDescriptorHeapSizeInfo(ID3D12Device* pDevice)
	{
		DescriptorHeapSizeInfo info{};
		info.RTVDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		info.DSVDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		info.CBV_SRV_UAVDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		info.SamplerDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		return info;
	}

	std::pair<std::unique_ptr<DescriptorHeap>, std::unique_ptr<DescriptorHeap>>
	Renderer::CreateDescriptorHeapsForSwapChainBuffers(ID3D12Device* pDevice)
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvDesc{};
		D3D12_DESCRIPTOR_HEAP_DESC dsvDesc{};

		rtvDesc.NumDescriptors = g_NUM_FRAMES;
		rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvDesc.NodeMask = 0;

		dsvDesc.NumDescriptors = 1;
		dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvDesc.NodeMask = 0;

		auto rtvDescriptorHeap = std::make_unique<DescriptorHeap>(
			pDevice, rtvDesc);
		auto dsvDescriptorHeap = std::make_unique<DescriptorHeap>(
			pDevice, dsvDesc);

		return {std::move(rtvDescriptorHeap), std::move(dsvDescriptorHeap)};
	}

	void Renderer::Render(FLOAT const* clearColor)
	{
		auto& commandAllocator = m_CmdAllocators[m_CurrentBackBufferIndex];
		auto& backBuffer = m_BackBuffers[m_CurrentBackBufferIndex];

		commandAllocator->Reset();
		m_GfxCmdList->Reset(commandAllocator->GetCommandAllocator(), nullptr);

		CD3DX12_RESOURCE_BARRIER presentToRTVBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_GfxCmdList->GetGraphicsCommandList()->ResourceBarrier(1UL, &presentToRTVBarrier);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
			m_RTVDescriptorHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), m_CurrentBackBufferIndex,
			m_DescriptorHeapSizeInfo.RTVDescriptorSize);

		m_GfxCmdList->GetGraphicsCommandList()->ClearRenderTargetView(rtv, clearColor, 0UL, nullptr);


		CD3DX12_RESOURCE_BARRIER RTVtoPresentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_GfxCmdList->GetGraphicsCommandList()->ResourceBarrier(1UL, &RTVtoPresentBarrier);
		D3D_CALL(m_GfxCmdList->GetGraphicsCommandList()->Close());

		ID3D12CommandList* const commandLists[] = { m_GfxCmdList->GetGraphicsCommandList() };
		m_CmdQueue->GetCommandQueue()->ExecuteCommandLists(1UL, commandLists);

		UINT syncInterval = m_PresentMethod.EnableVSync ? 1 : 0;
		UINT presentFlags = m_Device->IsTearingSupported() && !m_PresentMethod.EnableVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
		m_SwapChain->Present(syncInterval,presentFlags);

		m_FrameFenceValues[m_CurrentBackBufferIndex] = Synchronizer::IncrementAndSignal(
			m_CmdQueue->GetCommandQueue(), m_Fence->GetFence(), m_FenceValue);

		m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
		acc3d_trace(m_CurrentBackBufferIndex);
		Synchronizer::WaitForFenceValue(m_Fence->GetFence(), m_FrameFenceValues[m_CurrentBackBufferIndex], m_FenceEvent);
	}

	void Renderer::UpdateRenderTargetViews()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
			m_RTVDescriptorHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

		for (UINT i = 0; i < g_NUM_FRAMES; ++i)
		{
			ComPtr<ID3D12Resource> pBackBuffer;
			D3D_CALL(m_SwapChain->GetDXGISwapChain()->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer)));

			m_Device->GetD3D12Device()->CreateRenderTargetView(pBackBuffer.Get(), nullptr,
			                                                   rtvHandle);
			m_BackBuffers[i] = pBackBuffer;
			rtvHandle.Offset(static_cast<INT>(m_DescriptorHeapSizeInfo.RTVDescriptorSize));
		}
	}
}
