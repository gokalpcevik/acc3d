#include "Renderer.h"

namespace acc3d::Graphics
{
	using Microsoft::WRL::ComPtr;
	using namespace DirectX;

	void Renderer::Render(const FLOAT* clearColor)
	{
		auto& commandAllocator = m_CmdAllocators[m_CurrentBackBufferIndex];
		auto& backBuffer = m_BackBuffers[m_CurrentBackBufferIndex];

		commandAllocator->Reset();
		m_GfxCmdList->Reset(commandAllocator->GetCommandAllocator(), nullptr);

		CD3DX12_RESOURCE_BARRIER presentToRTVBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_GfxCmdList->GetGraphicsCommandList()->ResourceBarrier(1UL, &presentToRTVBarrier);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
			m_RTVDescriptorHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
			(m_CurrentBackBufferIndex),
			m_DescriptorHeapSizeInfo.RTVDescriptorSize);

		m_GfxCmdList->GetGraphicsCommandList()->ClearRenderTargetView(rtv, clearColor, 0UL,
		                                                              nullptr);

		CD3DX12_RESOURCE_BARRIER RTVtoPresentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_GfxCmdList->GetGraphicsCommandList()->ResourceBarrier(1UL, &RTVtoPresentBarrier);
		THROW_IFF(m_GfxCmdList->GetGraphicsCommandList()->Close());

		ID3D12CommandList* const commandLists[] = {m_GfxCmdList->GetGraphicsCommandList()};
		m_CmdQueue->GetD3D12CommandQueue()->ExecuteCommandLists(1UL, commandLists);


		UINT syncInterval = m_PresentMethod.EnableVSync ? 1 : 0;
		UINT presentFlags = m_Device->IsTearingSupported() && !m_PresentMethod.EnableVSync
			                    ? DXGI_PRESENT_ALLOW_TEARING
			                    : 0;
		m_SwapChain->Present(syncInterval, presentFlags);


		m_FrameFenceValues[m_CurrentBackBufferIndex] = Synchronizer::IncrementAndSignal(
			m_CmdQueue->GetD3D12CommandQueue(), m_Fence->GetFence(), m_FenceValue);


		m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

		Synchronizer::WaitForFenceValue(m_Fence->GetFence(),
		                                m_FrameFenceValues[m_CurrentBackBufferIndex],
		                                m_FenceEvent);
	}

	void Renderer::UpdateRenderTargetViews()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
			m_RTVDescriptorHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

		for (UINT i = 0; i < g_NUM_FRAMES_IN_FLIGHT; ++i)
		{
			ComPtr<ID3D12Resource> pBackBuffer;
			THROW_IFF(m_SwapChain->GetDXGISwapChain()->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer)));

			m_Device->GetD3D12Device()->CreateRenderTargetView(pBackBuffer.Get(), nullptr, rtvHandle);
			m_BackBuffers[i] = pBackBuffer;
			rtvHandle.Offset(static_cast<INT>(m_DescriptorHeapSizeInfo.RTVDescriptorSize));
		}
	}

	Renderer::~Renderer()
	{
		Synchronizer::Flush(m_CmdQueue->GetD3D12CommandQueue(), m_Fence->GetFence(), m_FenceValue,
		                    m_FenceEvent);
	}

	void Renderer::Resize(uint32_t
	                      width,
	                      uint32_t height
	)
	{
		Synchronizer::Flush(m_CmdQueue->GetD3D12CommandQueue(), m_Fence->GetFence(), m_FenceValue, m_FenceEvent);
		for (size_t i = 0;i < g_NUM_FRAMES_IN_FLIGHT;++i)
		{
			m_BackBuffers[i].
			Reset();
			m_FrameFenceValues[i] = m_FrameFenceValues[m_CurrentBackBufferIndex];
		}
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};

		THROW_IFF(m_SwapChain->GetDXGISwapChain()->GetDesc(&swapChainDesc));
		THROW_IFF(m_SwapChain->GetDXGISwapChain()->ResizeBuffers(g_NUM_FRAMES_IN_FLIGHT, width, height,
			swapChainDesc.BufferDesc.Format,
			swapChainDesc.Flags));

		m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
		this->UpdateRenderTargetViews();
	}

	const std::unique_ptr<Device>& Renderer::GetDevice() const
	{
		return m_Device;
	}

	const std::unique_ptr<Fence>& Renderer::GetFence() const
	{
		return m_Fence;
	}
}
