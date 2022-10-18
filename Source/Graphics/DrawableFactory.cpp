#include "DrawableFactory.h"

#include "Synchronizer.h"

namespace acc3d::Graphics
{
	using Microsoft::WRL::ComPtr;

	Drawable* DrawableFactory::CreateDrawable(
		ID3D12Device2* pDevice,
		D3D12MA::Allocator* pAllocator,
		ID3D12CommandQueue* pCopyCommandQueue,
		ID3D12Fence* pCopyFence,
		uint64_t& fenceValue,
		HANDLE fenceEvent,
		Asset::MeshAssetId assetId,
		RootSignatureInitializer const& rootSigInit)
	{
		const std::unique_ptr<CommandAllocator> m_LoadCmdAllocator = std::make_unique<CommandAllocator>(pDevice,
			D3D12_COMMAND_LIST_TYPE_COPY);
		const std::unique_ptr<CommandList> m_LoadCmdList = std::make_unique<CommandList>(
			pDevice, m_LoadCmdAllocator->GetD3D12CommandAllocatorPtr(),
			D3D12_COMMAND_LIST_TYPE_COPY, nullptr);

		// Reset the command list and set an open state as we will be recording copy commands for the drawable object.
		m_LoadCmdList->Reset(m_LoadCmdAllocator->GetD3D12CommandAllocatorPtr(), nullptr);

		const auto& cmdList = m_LoadCmdList->GetD3D12GraphicsCommandList();

		Drawable* const drawable = new Drawable();

		drawable->AssetId = assetId;
		drawable->RootSignatureId = rootSigInit.RootSignatureId;

		auto& [Vertices, Indices] = Asset::MeshLibrary::Retrieve(assetId);

		assert(!Vertices.empty() && !Indices.empty());

		drawable->IndicesCount = Indices.size();

		D3D12MA::ALLOCATION_DESC intermediateAllocDesc;
		intermediateAllocDesc.ExtraHeapFlags = D3D12_HEAP_FLAG_NONE;
		intermediateAllocDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
		intermediateAllocDesc.CustomPool = NULL;
		intermediateAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

		D3D12MA::ALLOCATION_DESC destAllocDesc;
		destAllocDesc.ExtraHeapFlags = D3D12_HEAP_FLAG_NONE;
		destAllocDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
		destAllocDesc.CustomPool = NULL;
		destAllocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		auto verticesResDesc = CD3DX12_RESOURCE_DESC::Buffer(Vertices.size() * sizeof(Vertex));
		auto indicesResDesc = CD3DX12_RESOURCE_DESC::Buffer(Indices.size() * sizeof(uint32_t));

		D3D12MA::Allocation* vbIntermediateAlloc;
		D3D12MA::Allocation* ibIntermediateAlloc;

		THROW_IFF(pAllocator->CreateResource(&destAllocDesc,
			&verticesResDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			&drawable->VertexBuffer,
			IID_NULL,
			NULL));

		THROW_IFF(pAllocator->CreateResource(&intermediateAllocDesc,
			&verticesResDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			&vbIntermediateAlloc,
			IID_NULL,
			NULL));

		D3D12_SUBRESOURCE_DATA verticesSubresourceData{};
		verticesSubresourceData.pData = Vertices.data();
		verticesSubresourceData.RowPitch = Vertices.size() * sizeof(Vertex);
		verticesSubresourceData.SlicePitch = verticesSubresourceData.RowPitch;

		::UpdateSubresources(cmdList.Get(),
			drawable->VertexBuffer->GetResource(),
			vbIntermediateAlloc->GetResource(), 0, 0, 1, &verticesSubresourceData);


		D3D12_SUBRESOURCE_DATA indicesSubresourceData{};

		indicesSubresourceData.pData = Indices.data();
		indicesSubresourceData.RowPitch = Indices.size() * sizeof(uint32_t);
		indicesSubresourceData.SlicePitch = indicesSubresourceData.RowPitch;

		THROW_IFF(pAllocator->CreateResource(&destAllocDesc,
			&indicesResDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			&drawable->IndexBuffer,
			IID_NULL,
			NULL));

		THROW_IFF(pAllocator->CreateResource(&intermediateAllocDesc,
			&indicesResDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			&ibIntermediateAlloc,
			IID_NULL,
			NULL));

		::UpdateSubresources(cmdList.Get(),
			drawable->IndexBuffer->GetResource(),
			ibIntermediateAlloc->GetResource(), 0, 0, 1, &indicesSubresourceData);

		drawable->VertexBufferView.BufferLocation = drawable->VertexBuffer->GetResource()->GetGPUVirtualAddress();
		drawable->VertexBufferView.SizeInBytes = static_cast<UINT>(Vertices.size() * sizeof(Vertex));
		drawable->VertexBufferView.StrideInBytes = static_cast<UINT>(sizeof(Vertex));

		drawable->IndexBufferView.BufferLocation = drawable->IndexBuffer->GetResource()->GetGPUVirtualAddress();
		drawable->IndexBufferView.SizeInBytes = static_cast<UINT>(Indices.size() * sizeof(uint32_t));
		drawable->IndexBufferView.Format = DXGI_FORMAT_R32_UINT;


		ShaderCompilationParameters const vertexShaderCompilationParams =
			ShaderCompilationParameters::Param_CompileVS_StdIncNoFlagsMainEntry(
				L"Shaders\\diffuse.vsh",
				static_cast<ShaderId>(SHADER_ID_ENTRY_VALUE::DIFFUSE_VERTEX_SHADER));

		ShaderCompilationParameters const pixelShaderCompilationParams =
			ShaderCompilationParameters::Param_CompilePS_StdIncNoFlagsMainEntry(
				L"Shaders\\diffuse.psh",
				static_cast<ShaderId>(SHADER_ID_ENTRY_VALUE::DIFFUSE_PIXEL_SHADER));

		auto [VertexShaderId, VertexShaderEntry] = ShaderLibrary::CompileAndLoad(vertexShaderCompilationParams);
		auto [PixelShaderId, PixelShaderEntry] = ShaderLibrary::CompileAndLoad(pixelShaderCompilationParams);

		RootSignature const* pRootSignature = RootSignatureLibrary::CreateRootSignatureEntry(pDevice, rootSigInit);

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

		VertexLayout layout(VertexShaderId);

		D3D12_RESOURCE_DESC1 desc;

		auto inputLayout = layout.GetD3D12InputLayout();
		pipelineStateStream.pRootSignature = pRootSignature->GetD3D12RootSignaturePtr();
		pipelineStateStream.InputLayout = { inputLayout.data(), static_cast<UINT>(inputLayout.size()) };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(VertexShaderEntry.Blob.Get());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(PixelShaderEntry.Blob.Get());
		pipelineStateStream.RTVFormats = rtvFormats;
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(PipelineStateStream), &pipelineStateStream };

		drawable->PipelineState = std::make_unique<PipelineState>(pDevice, &pipelineStateStreamDesc);

		m_LoadCmdList->GetD3D12GraphicsCommandListPtr()->Close();
		ID3D12CommandList* const commandLists[] = { m_LoadCmdList->GetD3D12GraphicsCommandListPtr() };

		pCopyCommandQueue->ExecuteCommandLists(1UL, commandLists);
		Synchronizer::IncrementAndSignal(pCopyCommandQueue, pCopyFence, fenceValue);
		Synchronizer::WaitForFenceValue(pCopyFence, fenceValue, fenceEvent);

		vbIntermediateAlloc->Release();
		ibIntermediateAlloc->Release();

		return drawable;
	}
}
