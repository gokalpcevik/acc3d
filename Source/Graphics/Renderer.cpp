#include "Renderer.h"

namespace acc3d::Graphics
{
	using Microsoft::WRL::ComPtr;
	using namespace DirectX;

	void Renderer::Clear(const FLOAT* clearColor)
	{
		// Update the model matrix.

		static float time = 0.0f;
		time += 1.f;
		float angle = time;
		const XMVECTOR rotationAxis = XMVectorSet(1, 1, 1, 0);
		m_CubeInfo.ModelMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));

		// Update the view matrix.
		const XMVECTOR eyePosition = XMVectorSet(0, 0, -10, 1);
		const XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
		const XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
		m_CubeInfo.ViewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);

		// Update the projection matrix.
		float aspectRatio = (float)m_Window->GetSurfaceWidth() / m_Window->GetSurfaceHeight();
		m_CubeInfo.ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_CubeInfo.FOV), aspectRatio, 0.1f, 100.0f);


/*
 * Get references/pointers to the variable that will be used throughout the function.
 */
		auto& commandAllocator = m_GfxCmdAllocators[m_CurrentBackBufferIndex];
		auto& backBuffer = m_BackBuffers[m_CurrentBackBufferIndex];
		auto cmdList = m_GfxCmdList->GetD3D12GraphicsCommandListPtr();

		commandAllocator->Reset();
		m_GfxCmdList->Reset(commandAllocator->GetD3D12CommandAllocatorPtr(), nullptr);



/*
 * Switch the resource state to render target state.
 */
		backBuffer->TransitionAndBarrier(cmdList, D3D12_RESOURCE_STATE_PRESENT,
		                                 D3D12_RESOURCE_STATE_RENDER_TARGET);


/*
 * -----------------------------------CLEAR-------------------------------------------------------
 */
		/*
		 * Get the CPU descriptor handle from the descriptor heap and clear the current frame render target view.
		 */
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
			m_RTVDescriptorHeap->GetD3D12DescriptorHeapPtr()->GetCPUDescriptorHandleForHeapStart(),
			(m_CurrentBackBufferIndex),
			m_DescriptorHeapSizeInfo.RTVDescriptorSize);

		this->ClearRenderTargetView(rtv, clearColor);

		/*
		* Get the CPU descriptor handle from the descriptor heap and clear the depth stencil view.
		*/
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsv(m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());


		this->ClearDepthStencilView(dsv);
/*
* -----------------------------------------------------------------------------------------------
*/
		cmdList->SetPipelineState(m_CubeInfo.PipelineState.Get());
		cmdList->SetGraphicsRootSignature(m_CubeInfo.RootSignature->GetD3D12RootSignaturePtr());
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		cmdList->IASetVertexBuffers(0, 1, &m_CubeInfo.VertexBufferView);
		cmdList->IASetIndexBuffer(&m_CubeInfo.IndexBufferView);

		cmdList->RSSetViewports(1, &m_Viewport);
		cmdList->RSSetScissorRects(1, &m_ScissorRect);

		D3D12_CPU_DESCRIPTOR_HANDLE const depthStencilDescriptor = m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		cmdList->OMSetRenderTargets(1, &rtv, FALSE, &depthStencilDescriptor);

		// Update the MVP matrix
		XMMATRIX mvpMatrix = XMMatrixMultiply(m_CubeInfo.ModelMatrix, m_CubeInfo.ViewMatrix);
		mvpMatrix = XMMatrixMultiply(mvpMatrix, m_CubeInfo.ProjectionMatrix);
		cmdList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvpMatrix, 0);

		cmdList->DrawIndexedInstanced(m_CubeInfo.indicesCount, 1, 0, 0, 0);

/*
 * Switch the backbuffer state to present as we will be sending a present command to the swapchain.
 */
		backBuffer->TransitionAndBarrier(cmdList, D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);

		THROW_IFF(cmdList->Close());

		ID3D12CommandList* const commandLists[] = { cmdList };
		m_DirectCmdQueue->GetD3D12CommandQueuePtr()->ExecuteCommandLists(1UL, commandLists);


		UINT syncInterval = m_PresentMethod.EnableVSync ? 1 : 0;
		UINT presentFlags = m_Device->IsTearingSupported() && !m_PresentMethod.EnableVSync
			                    ? DXGI_PRESENT_ALLOW_TEARING
			                    : 0;
		m_SwapChain->Present(syncInterval, presentFlags);

/*
 * Wait if the next frame is not ready to begin yet.
 */
		m_FrameFenceValues[m_CurrentBackBufferIndex] = Synchronizer::IncrementAndSignal(
			m_DirectCmdQueue->GetD3D12CommandQueuePtr(), m_Fence->GetD3D12FencePtr(), m_FenceValue);

		m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

		Synchronizer::WaitForFenceValue(m_Fence->GetD3D12FencePtr(),
		                                m_FrameFenceValues[m_CurrentBackBufferIndex],
		                                m_FenceEvent);

#if defined(_DEBUG) || defined(DEBUG)
		m_InfoQueue->FlushQueue();
#endif
	}

	void Renderer::UpdateRenderTargetViews()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
			m_RTVDescriptorHeap->GetD3D12DescriptorHeapPtr()->GetCPUDescriptorHandleForHeapStart());

		for (UINT i = 0; i < g_NUM_FRAMES_IN_FLIGHT; ++i)
		{
			ComPtr<ID3D12Resource> pBackBuffer;
			THROW_IFF(m_SwapChain->GetDXGISwapChain()->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer)));

			m_Device->GetD3D12DevicePtr()->CreateRenderTargetView(pBackBuffer.Get(), nullptr, rtvHandle);
			m_BackBuffers[i].reset();
			m_BackBuffers[i] = std::make_unique<Resource>(pBackBuffer);
			rtvHandle.Offset(static_cast<INT>(m_DescriptorHeapSizeInfo.RTVDescriptorSize));
		}
	
	}

	void Renderer::ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT const* clearColor)
	{
		m_GfxCmdList->GetD3D12GraphicsCommandListPtr()->ClearRenderTargetView(rtv, clearColor, 1UL,
			&m_ScissorRect);
	}

	void Renderer::ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE dsv, D3D12_CLEAR_FLAGS flags, FLOAT depth)
	{
		m_GfxCmdList->GetD3D12GraphicsCommandListPtr()->ClearDepthStencilView(
			dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 1UL, &m_ScissorRect);
	}

	void Renderer::RenderScene(ECS::Scene& scene)
	{
		auto group = scene.GetEnTTRegistry().group_if_exists<ECS::TransformComponent,ECS::MeshRendererComponent>();
		
		for (auto entity : group)
		{
			auto [tc, mrc] = group.get(entity);
			auto* drawable = m_DrawableMap[ECS::RIDAccessor()(mrc)];
		}
	}


	Renderer::~Renderer()
	{
		Synchronizer::Flush(m_DirectCmdQueue->GetD3D12CommandQueuePtr(), m_Fence->GetD3D12FencePtr(), m_FenceValue,
		                    m_FenceEvent);
	}

	void Renderer::Resize(uint32_t width,uint32_t height)
	{
		Synchronizer::Flush(m_DirectCmdQueue->GetD3D12CommandQueuePtr(), m_Fence->GetD3D12FencePtr(), m_FenceValue, m_FenceEvent);
		for (size_t i = 0; i < g_NUM_FRAMES_IN_FLIGHT; ++i)
		{
			m_BackBuffers[i]->GetResource().Reset();
			m_FrameFenceValues[i] = m_FrameFenceValues[m_CurrentBackBufferIndex];
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};

		THROW_IFF(m_SwapChain->GetDXGISwapChain()->GetDesc(&swapChainDesc));
		THROW_IFF(m_SwapChain->GetDXGISwapChain()->ResizeBuffers(g_NUM_FRAMES_IN_FLIGHT, width, height,
			swapChainDesc.BufferDesc.Format,
			swapChainDesc.Flags));

		m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

		m_Viewport.Width = static_cast<FLOAT>(m_Window->GetSurfaceWidth());
		m_Viewport.Height = static_cast<FLOAT>(m_Window->GetSurfaceHeight());
		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;
		this->UpdateRenderTargetViews();
		this->ResizeDepthBuffer(m_Window->GetSurfaceWidth(), m_Window->GetSurfaceHeight());

		for (size_t i = 0; i < g_NUM_FRAMES_IN_FLIGHT; ++i)
		{
			m_BackBuffers[i]->GetResource()->SetName(fmt::format(L"Backbuffer:{0}",i).c_str());
		}
	}

	void Renderer::ResizeDepthBuffer(uint32_t width, uint32_t height)
	{
		D3D12_CLEAR_VALUE optimizedClearValue{};
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 1.0f, 0 };

		auto const depthBufferHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto const depthBufferResourceDescription = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, 1600, 900, 1, 0, 1, 0,
		                                                          D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		m_DepthBuffer.reset();
		m_DepthBuffer = std::make_unique<Resource>(m_Device->GetD3D12DevicePtr(), &depthBufferHeapProperties,
		                                           D3D12_HEAP_FLAG_NONE, &depthBufferResourceDescription,
		                                           D3D12_RESOURCE_STATE_DEPTH_WRITE, &optimizedClearValue);
		D3D12_DEPTH_STENCIL_VIEW_DESC DSVDescription = {};
		DSVDescription.Format = DXGI_FORMAT_D32_FLOAT;
		DSVDescription.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		DSVDescription.Texture2D.MipSlice = 0;
		DSVDescription.Flags = D3D12_DSV_FLAG_NONE;
		m_Device->GetD3D12Device2()->CreateDepthStencilView(m_DepthBuffer->GetResourcePtr(), &DSVDescription,
		                                                    m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	}

	const std::unique_ptr<Device>& Renderer::GetDevice() const
	{
		return m_Device;
	}

	

	void Renderer::LoadTestScene()
	{
		auto pDevice = m_Device->GetD3D12Device2();

		m_CubeInfo.FOV = 45.0f;

		std::unique_ptr<CommandList> m_LoadCmdList;
		std::unique_ptr<CommandAllocator> m_LoadCmdAllocator;

		m_LoadCmdAllocator = std::make_unique<CommandAllocator>(pDevice.Get(), D3D12_COMMAND_LIST_TYPE_COPY);
		m_LoadCmdList = std::make_unique<CommandList>(pDevice.Get(), m_LoadCmdAllocator->GetD3D12CommandAllocatorPtr(),
			D3D12_COMMAND_LIST_TYPE_COPY, nullptr);
		m_LoadCmdList->Reset(m_LoadCmdAllocator->GetD3D12CommandAllocatorPtr(), nullptr);


		auto[id,meshData] = Asset::MeshLibrary::Load("Assets/donut.obj");
		m_CubeInfo.indicesCount = meshData.Indices.size();

		ComPtr<ID3D12Resource> intermediateVertexBuffer;
		Resource::UpdateBufferResource(m_Device->GetD3D12DevicePtr(), m_LoadCmdList->GetD3D12GraphicsCommandListPtr(),
		                               &m_CubeInfo.VertexBuffer, &intermediateVertexBuffer,
		                               meshData.Vertices.size(), sizeof(Vertex), meshData.Vertices.data());

		m_CubeInfo.VertexBufferView.BufferLocation = m_CubeInfo.VertexBuffer->GetGPUVirtualAddress();
		m_CubeInfo.VertexBufferView.SizeInBytes = meshData.Vertices.size() * sizeof(Vertex);
		m_CubeInfo.VertexBufferView.StrideInBytes = sizeof(Vertex);

		ComPtr<ID3D12Resource> intermediateIndexBuffer;

		Resource::UpdateBufferResource(m_Device->GetD3D12DevicePtr(), m_LoadCmdList->GetD3D12GraphicsCommandListPtr(),
		                               &m_CubeInfo.IndexBuffer, &intermediateIndexBuffer,
		                               meshData.Indices.size(), sizeof(uint32_t), meshData.Indices.data());

		m_CubeInfo.IndexBufferView.BufferLocation = m_CubeInfo.IndexBuffer->GetGPUVirtualAddress();
		m_CubeInfo.IndexBufferView.SizeInBytes = meshData.Indices.size() * sizeof(uint32_t);
		m_CubeInfo.IndexBufferView.Format = DXGI_FORMAT_R32_UINT;

		ShaderCompilationParameters const vertexShaderCompilationParams =
			ShaderCompilationParameters::Param_CompileVS_StdIncNoFlagsMainEntry(L"Shaders\\vertex.vsh");

		ShaderCompilationParameters const pixelShaderCompilationParams =
			ShaderCompilationParameters::Param_CompilePS_StdIncNoFlagsMainEntry(L"Shaders\\pixel.psh");

		auto&& [VertexShaderId, VertexShaderEntry] = ShaderLibrary::CompileAndLoad(vertexShaderCompilationParams);
		auto&& [PixelShaderId,PixelShaderEntry] =  ShaderLibrary::CompileAndLoad(pixelShaderCompilationParams);

		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		CD3DX12_ROOT_PARAMETER1 rootParameters[1];
		rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

		auto [rootSignatureBlob,rootSignatureErrorBlob] =
			RootSignature::SerializeVersionedRootSignatureWithHighestVersion(pDevice.Get(), rootSignatureDescription);

		m_CubeInfo.RootSignature = std::make_unique<RootSignature>(pDevice.Get(), rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize());

		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		} pipelineStateStream;

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		pipelineStateStream.pRootSignature = m_CubeInfo.RootSignature->GetD3D12RootSignaturePtr();
		pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(VertexShaderEntry.Blob.Get());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(PixelShaderEntry.Blob.Get());
		pipelineStateStream.RTVFormats = rtvFormats;
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
		sizeof(PipelineStateStream), &pipelineStateStream
		};
		THROW_IFF(pDevice->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_CubeInfo.PipelineState)));

		m_LoadCmdList->GetD3D12GraphicsCommandListPtr()->Close();
		ID3D12CommandList* const commandLists[] = { m_LoadCmdList->GetD3D12GraphicsCommandListPtr() };
		m_CopyCmdQueue->GetD3D12CommandQueuePtr()->ExecuteCommandLists(1UL, commandLists);
		Synchronizer::IncrementAndSignal(m_CopyCmdQueue->GetD3D12CommandQueuePtr(), m_Fence->GetD3D12FencePtr(), m_FenceValue);
		Synchronizer::WaitForFenceValue(m_Fence->GetD3D12FencePtr(), m_FenceValue, m_FenceEvent);
	}

	RendererId Renderer::GenerateRendererId()
	{
		return m_RendererIdValue++;
	}

	void Renderer::RegisterMeshRendererComponentDrawable(RendererId id,Asset::MeshAssetId meshAssetId)
	{
		ComPtr<ID3D12Device2> const& device = m_Device->GetD3D12Device2();

		const std::unique_ptr<CommandAllocator> m_LoadCmdAllocator = std::make_unique<CommandAllocator>(device.Get(),
		                                                                                                D3D12_COMMAND_LIST_TYPE_COPY);
		const std::unique_ptr<CommandList> m_LoadCmdList = std::make_unique<CommandList>(
			device.Get(), m_LoadCmdAllocator->GetD3D12CommandAllocatorPtr(),
			D3D12_COMMAND_LIST_TYPE_COPY, nullptr);

		// Reset the command list and set an open state as we will be recording copy commands for the drawable object.
		m_LoadCmdList->Reset(m_LoadCmdAllocator->GetD3D12CommandAllocatorPtr(), nullptr);

		const auto& cmdList = m_LoadCmdList->GetD3D12GraphicsCommandList();

		Drawable* drawable = new Drawable();
		auto& [Vertices, Indices] = Asset::MeshLibrary::Retrieve(meshAssetId);

		assert(Vertices.size() > 0 && Indices.size() > 0);

		drawable->VertexBuffer = std::make_unique<Resource>(device.Get());
		drawable->IndexBuffer = std::make_unique<Resource>(device.Get());

		const std::unique_ptr vertexBufferIntermediateResource = std::make_unique<Resource>(device.Get());
		Resource::UpdateBufferResource(device.Get(), cmdList.Get(), *drawable->VertexBuffer, *vertexBufferIntermediateResource,
			Vertices.size(), sizeof(Vertex), Vertices.data());

		const std::unique_ptr indexBufferIntermediateResource = std::make_unique<Resource>(device.Get());
		Resource::UpdateBufferResource(device.Get(), cmdList.Get(), *drawable->IndexBuffer, *indexBufferIntermediateResource,
			Indices.size(), sizeof(uint32_t), Indices.data());
		
		drawable->VertexBufferView.BufferLocation = drawable->VertexBuffer->GetGPUVirtualAddress();
		drawable->VertexBufferView.SizeInBytes = Vertices.size() * sizeof(Vertex);
		drawable->VertexBufferView.StrideInBytes = sizeof(Vertex);

		drawable->IndexBufferView.BufferLocation = drawable->IndexBuffer->GetGPUVirtualAddress();
		drawable->IndexBufferView.SizeInBytes = Indices.size() * sizeof(uint32_t);
		drawable->IndexBufferView.Format = DXGI_FORMAT_R32_UINT;


		m_LoadCmdList->GetD3D12GraphicsCommandListPtr()->Close();
		ID3D12CommandList* const commandLists[] = { m_LoadCmdList->GetD3D12GraphicsCommandListPtr() };

		Synchronizer::Flush(m_DirectCmdQueue->GetD3D12CommandQueuePtr(), m_Fence->GetD3D12FencePtr(),
			m_FenceValue, m_FenceEvent);

		m_CopyCmdQueue->GetD3D12CommandQueuePtr()->ExecuteCommandLists(1UL, commandLists);
		Synchronizer::IncrementAndSignal(m_CopyCmdQueue->GetD3D12CommandQueuePtr(), m_CopyFence->GetD3D12FencePtr(), m_CopyFenceValue);
		Synchronizer::WaitForFenceValue(m_CopyFence->GetD3D12FencePtr(), m_CopyFenceValue, m_CopyFenceEvent);

		m_DrawableMap[id] = drawable;
	}

	void Renderer::DeregisterMeshRendererComponentDrawable(RendererId id)
	{
		Synchronizer::Flush(m_DirectCmdQueue->GetD3D12CommandQueuePtr(), m_Fence->GetD3D12FencePtr(),
			m_FenceValue, m_FenceEvent);

		delete m_DrawableMap[id];
	}
}
