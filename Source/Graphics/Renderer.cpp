#include "Renderer.h"

namespace acc3d::Graphics
{
	using Microsoft::WRL::ComPtr;
	using namespace DirectX;

	void Renderer::Clear(const FLOAT* clearColor) const
	{
		m_FrameDrawCallCount = 0;

/*
 * ------------------------RESOURCE BARRIER / MEMBER VAR ALIASES----------------------------------
 */
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
 * -----------------------------------------------------------------------------------------------
 */
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
	}

	void Renderer::Present()
	{
		ID3D12GraphicsCommandList2* gfxCmdList = m_GfxCmdList->GetD3D12GraphicsCommandListPtr();
		std::unique_ptr<Resource> const& backBuffer = m_BackBuffers[m_CurrentBackBufferIndex];

		backBuffer->TransitionAndBarrier(gfxCmdList, D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);

		THROW_IFF(gfxCmdList->Close());

		ID3D12CommandList* const commandLists[] = { gfxCmdList };
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


	void Renderer::RenderScene(ECS::Scene& scene)
	{
		auto const cameraView = scene.GetEnTTRegistryMutable().view<ECS::CameraComponent>();
		ECS::CameraComponent const* cameraComponent = nullptr;

		if(cameraView.size() == 0)
		{
			acc3d_warn("There is no camera to render the scene with.");
			return;
		}

		for (auto const entity : cameraView)
		{
			auto const& cc = cameraView.get<ECS::CameraComponent>(entity);
			if (cc.IsPrimaryCamera)
			{
				cameraComponent = &cc;
				break;
			}
		}

		if(cameraComponent == nullptr)
		{
			acc3d_warn("There is no primary camera set to render the scene with.");
			return;
		}

		ComPtr<ID3D12GraphicsCommandList2> const& gfxCmdList = m_GfxCmdList->GetD3D12GraphicsCommandList();

		const CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
			m_RTVDescriptorHeap->GetD3D12DescriptorHeapPtr()->GetCPUDescriptorHandleForHeapStart(),
			m_CurrentBackBufferIndex,
			m_DescriptorHeapSizeInfo.RTVDescriptorSize);

		D3D12_CPU_DESCRIPTOR_HANDLE const depthStencilDescriptor = m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		gfxCmdList->OMSetRenderTargets(1, &rtv, FALSE, &depthStencilDescriptor);


		ID3D12DescriptorHeap* const descriptorHeaps[] = { m_LightContext->GetDescriptorHeap(m_CurrentBackBufferIndex)->GetD3D12DescriptorHeapPtr() };

		m_LightContext->SetLightEntriesDefault(m_CurrentBackBufferIndex);


		auto const lightView = scene.GetEnTTRegistryMutable().view<ECS::DirectionalLightComponent>();

		for(size_t i = 0; i < lightView.size(); ++i)
		{
			if (i >= g_MAX_NUM_OF_DIR_LIGHTS) break;
			auto const entity = lightView[i];
			auto entry = scene.GetComponent<ECS::DirectionalLightComponent>(entity);
			m_LightContext->SetLightEntry(entry, m_CurrentBackBufferIndex, i);
			
		}

		float const aspectRatio = m_Viewport.Width / m_Viewport.Height;

		gfxCmdList->RSSetViewports(1, &m_Viewport);
		gfxCmdList->RSSetScissorRects(1, &m_ScissorRect);


		const auto lightGPUHandle = m_LightContext->GetDescriptorHeap(m_CurrentBackBufferIndex)->
		                                            GetD3D12DescriptorHeapPtr()->GetGPUDescriptorHandleForHeapStart();

		RootSignature const* pRootSignature = RootSignatureLibrary::Get(static_cast<RootSignatureId>(ROOT_SIG_ENTRY_VALUE::DIFFUSE_ROOT_SIGNATURE));
		gfxCmdList->SetGraphicsRootSignature(pRootSignature->GetD3D12RootSignaturePtr());
		
		// Directional Light data
		gfxCmdList->SetDescriptorHeaps(1UL, descriptorHeaps);

		gfxCmdList->SetGraphicsRootDescriptorTable(0UL, lightGPUHandle);

		const auto meshRenderView = scene.GetEnTTRegistryMutable().view<ECS::MeshRendererComponent, ECS::TransformComponent>();

		for (const auto entity : meshRenderView)
		{
			// tc:TransformComponent, mrc:MeshRendererComponent
			auto [mrc,tc] = meshRenderView.get(entity);
			Drawable const* drawable = m_DrawableMap[ECS::RIDAccessor()(mrc)];

			gfxCmdList->SetPipelineState(drawable->PipelineState->GetD3D12PipelineState().Get());
			gfxCmdList->IASetVertexBuffers(0UL, 1UL, &drawable->VertexBufferView);
			gfxCmdList->IASetIndexBuffer(&drawable->IndexBufferView);

			gfxCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			
			struct
			{
				XMMATRIX ModelViewProjection;
				XMMATRIX Model;
			} MVP_MODEL;

			MVP_MODEL.ModelViewProjection = XMMatrixMultiply(tc.GetTransformationMatrix(), cameraComponent->GetViewMatrix());
			MVP_MODEL.ModelViewProjection = XMMatrixMultiply(MVP_MODEL.ModelViewProjection, cameraComponent->GetProjectionMatrix(aspectRatio));

			MVP_MODEL.Model = tc.GetTransformationMatrix();

			// drawable->Material->SetGraphicsRootParameters(void* MVP_MODEL, XMFLOAT4 cameraPosition);

			// Model-View-Projection and Model matrices
			gfxCmdList->SetGraphicsRoot32BitConstants(1UL, 2 * sizeof(XMMATRIX) / 4, &MVP_MODEL, 0);
			
			gfxCmdList->DrawIndexedInstanced(drawable->IndicesCount, 1, 0, 0, 0);
			m_FrameDrawCallCount++;
		}
	}

	void Renderer::InitializeImGui()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO const& io = ImGui::GetIO(); (void)io;

		io.Fonts->AddFontFromFileTTF("Assets/Fonts/Bitter-Medium.ttf", 22);
		constexpr auto ColorFromBytes = [](uint8_t r, uint8_t g, uint8_t b)
		{
			return ImVec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 1.0f);
		};

		auto& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		const ImVec4 bgColor = ColorFromBytes(37, 37, 38);
		const ImVec4 lightBgColor = ColorFromBytes(82, 82, 85);
		const ImVec4 veryLightBgColor = ColorFromBytes(90, 90, 95);

		const ImVec4 panelColor = ColorFromBytes(51, 51, 55);
		const ImVec4 panelHoverColor = ColorFromBytes(29, 151, 236);
		const ImVec4 panelActiveColor = ColorFromBytes(0, 119, 200);

		const ImVec4 textColor = ColorFromBytes(255, 255, 255);
		const ImVec4 textDisabledColor = ColorFromBytes(151, 151, 151);
		const ImVec4 borderColor = ColorFromBytes(78, 78, 78);

		colors[ImGuiCol_Text] = textColor;
		colors[ImGuiCol_TextDisabled] = textDisabledColor;
		colors[ImGuiCol_TextSelectedBg] = panelActiveColor;
		colors[ImGuiCol_WindowBg] = bgColor;
		colors[ImGuiCol_ChildBg] = bgColor;
		colors[ImGuiCol_PopupBg] = bgColor;
		colors[ImGuiCol_Border] = borderColor;
		colors[ImGuiCol_BorderShadow] = borderColor;
		colors[ImGuiCol_FrameBg] = panelColor;
		colors[ImGuiCol_FrameBgHovered] = panelHoverColor;
		colors[ImGuiCol_FrameBgActive] = panelActiveColor;
		colors[ImGuiCol_TitleBg] = bgColor;
		colors[ImGuiCol_TitleBgActive] = bgColor;
		colors[ImGuiCol_TitleBgCollapsed] = bgColor;
		colors[ImGuiCol_MenuBarBg] = panelColor;
		colors[ImGuiCol_ScrollbarBg] = panelColor;
		colors[ImGuiCol_ScrollbarGrab] = lightBgColor;
		colors[ImGuiCol_ScrollbarGrabHovered] = veryLightBgColor;
		colors[ImGuiCol_ScrollbarGrabActive] = veryLightBgColor;
		colors[ImGuiCol_CheckMark] = panelActiveColor;
		colors[ImGuiCol_SliderGrab] = panelHoverColor;
		colors[ImGuiCol_SliderGrabActive] = panelActiveColor;
		colors[ImGuiCol_Button] = panelColor;
		colors[ImGuiCol_ButtonHovered] = panelHoverColor;
		colors[ImGuiCol_ButtonActive] = panelHoverColor;
		colors[ImGuiCol_Header] = panelColor;
		colors[ImGuiCol_HeaderHovered] = panelHoverColor;
		colors[ImGuiCol_HeaderActive] = panelActiveColor;
		colors[ImGuiCol_Separator] = borderColor;
		colors[ImGuiCol_SeparatorHovered] = borderColor;
		colors[ImGuiCol_SeparatorActive] = borderColor;
		colors[ImGuiCol_ResizeGrip] = bgColor;
		colors[ImGuiCol_ResizeGripHovered] = panelColor;
		colors[ImGuiCol_ResizeGripActive] = lightBgColor;
		colors[ImGuiCol_PlotLines] = panelActiveColor;
		colors[ImGuiCol_PlotLinesHovered] = panelHoverColor;
		colors[ImGuiCol_PlotHistogram] = panelActiveColor;
		colors[ImGuiCol_PlotHistogramHovered] = panelHoverColor;
		colors[ImGuiCol_DragDropTarget] = bgColor;
		colors[ImGuiCol_NavHighlight] = bgColor;
		colors[ImGuiCol_Tab] = bgColor;
		colors[ImGuiCol_TabActive] = panelActiveColor;
		colors[ImGuiCol_TabUnfocused] = bgColor;
		colors[ImGuiCol_TabUnfocusedActive] = panelActiveColor;
		colors[ImGuiCol_TabHovered] = panelHoverColor;

		style.WindowRounding = 0.0f;
		style.ChildRounding = 0.0f;
		style.FrameRounding = 0.0f;
		style.GrabRounding = 0.0f;
		style.PopupRounding = 0.0f;
		style.ScrollbarRounding = 0.0f;
		style.TabRounding = 0.0f;

		this->CreateImGuiFontDescriptorHeap();

		D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle = m_ImGuiFontDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle = m_ImGuiFontDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

		ImGui_ImplSDL2_InitForD3D(m_Window->GetSDLWindow());
		ImGui_ImplDX12_Init(m_Device->GetD3D12DevicePtr(),
			g_NUM_FRAMES_IN_FLIGHT,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			m_ImGuiFontDescriptorHeap,
			CPUDescriptorHandle,
			GPUDescriptorHandle);
	}

	void Renderer::RenderImGui() const
	{
		ID3D12GraphicsCommandList2* gfxCmdList = m_GfxCmdList->GetD3D12GraphicsCommandListPtr();

		ID3D12DescriptorHeap* const imguiDescriptorHeap[] = { m_ImGuiFontDescriptorHeap };
		gfxCmdList->SetDescriptorHeaps(1UL, imguiDescriptorHeap);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), gfxCmdList);
	}

	size_t Renderer::GetFrameDrawCallCount() const
	{
		return m_FrameDrawCallCount;
	}

	void Renderer::CreateImGuiFontDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		THROW_IFF(m_Device->GetD3D12Device2()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_ImGuiFontDescriptorHeap)));
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
			m_BackBuffers[i] = std::make_unique<Resource>(pBackBuffer);
			rtvHandle.Offset(static_cast<INT>(m_DescriptorHeapSizeInfo.RTVDescriptorSize));
		}

	}

	void Renderer::ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE rtv, FLOAT const* clearColor) const
	{
		m_GfxCmdList->GetD3D12GraphicsCommandListPtr()->ClearRenderTargetView(rtv, clearColor, 1UL,
			&m_ScissorRect);
	}

	void Renderer::ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE dsv, D3D12_CLEAR_FLAGS flags, FLOAT depth) const
	{
		m_GfxCmdList->GetD3D12GraphicsCommandListPtr()->ClearDepthStencilView(
			dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 1UL, &m_ScissorRect);
	}

	void Renderer::Resize(uint32_t width,uint32_t height)
	{
		Synchronizer::Flush(m_DirectCmdQueue->GetD3D12CommandQueuePtr(), m_Fence->GetD3D12FencePtr(), m_FenceValue, m_FenceEvent);
		for (size_t i = 0; i < g_NUM_FRAMES_IN_FLIGHT; ++i)
		{
			m_BackBuffers[i]->GetResource().Reset();
			m_FrameFenceValues[i] = m_FrameFenceValues[m_CurrentBackBufferIndex];
		}
		m_DepthBuffer->Release();
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
	}

	void Renderer::ResizeDepthBuffer(uint32_t width, uint32_t height)
	{
		D3D12_CLEAR_VALUE optimizedClearValue{};
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 1.0f, 0 };

		auto const depthBufferHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto const depthBufferResourceDescription = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, 1600, 900, 1, 0, 1, 0,
		                                                          D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		D3D12MA::ALLOCATION_DESC allocationDesc;
		allocationDesc.CustomPool = nullptr;
		allocationDesc.ExtraHeapFlags = D3D12_HEAP_FLAG_NONE;
		allocationDesc.Flags = D3D12MA::ALLOCATION_FLAG_COMMITTED;
		allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		m_Allocator->CreateResource(&allocationDesc,
			&depthBufferResourceDescription,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optimizedClearValue,
			&m_DepthBuffer, IID_NULL, NULL);

		D3D12_DEPTH_STENCIL_VIEW_DESC DSVDescription = {};
		DSVDescription.Format = DXGI_FORMAT_D32_FLOAT;
		DSVDescription.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		DSVDescription.Texture2D.MipSlice = 0;
		DSVDescription.Flags = D3D12_DSV_FLAG_NONE;
		m_Device->GetD3D12Device2()->CreateDepthStencilView(m_DepthBuffer->GetResource(), &DSVDescription,
		                                                    m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	}

	const std::unique_ptr<Device>& Renderer::GetDevice() const
	{
		return m_Device;
	}

	D3D12MA::Stats Renderer::GetAllocatorStats() const
	{
		D3D12MA::Stats stats;
		m_Allocator->CalculateStats(&stats);
		return stats;
	}


	RendererId Renderer::GenerateRendererId()
	{
		if (m_RendererIdValue == std::numeric_limits<uint64_t>::max())
			m_RendererIdValue = 2;
		return m_RendererIdValue++;
	}

	void Renderer::RegisterMeshRendererComponentDrawable(Asset::MeshAssetId meshAssetId,RendererId& assignedRendererId, RootSignatureInitializer const& rootSigInit)
	{
		Drawable* drawable = DrawableFactory::CreateDrawable(m_Device->GetD3D12Device2().Get(),
			m_Allocator,
			m_CopyCmdQueue->GetD3D12CommandQueuePtr(),
			m_CopyFence->GetD3D12FencePtr(),
			m_CopyFenceValue,
			m_CopyFenceEvent,
			meshAssetId,
			rootSigInit);

		drawable->RendererId = (assignedRendererId = this->GenerateRendererId());
		m_DrawableMap[drawable->RendererId] = drawable;
	}


	void Renderer::DeregisterMeshRendererComponentDrawable(RendererId id)
	{
		Synchronizer::Flush(m_DirectCmdQueue->GetD3D12CommandQueuePtr(), m_Fence->GetD3D12FencePtr(), m_FenceValue,
			m_FenceEvent);
		Drawable const* drawable = m_DrawableMap[id];
		drawable->VertexBuffer->Release();
		drawable->IndexBuffer->Release();
		delete m_DrawableMap[id];
	}

	void Renderer::Shutdown()
	{
		Synchronizer::Flush(m_DirectCmdQueue->GetD3D12CommandQueuePtr(), m_Fence->GetD3D12FencePtr(), m_FenceValue,
			m_FenceEvent);
		m_LightContext->Release();
		m_DepthBuffer->Release();
		m_Allocator->Release();

		ImGui_ImplDX12_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	void Renderer::InitDrawableDenseHashMap()
	{
		m_DrawableMap.set_empty_key(RENDERER_ID_EMPTY_KEY_VALUE);
		m_DrawableMap.set_deleted_key(RENDERER_ID_DELETED_KEY_VALUE);
	}
}
