#include "Resource.h"

namespace acc3d::Graphics
{
	using Microsoft::WRL::ComPtr;

	Resource::Resource(ID3D12Device* pDevice)
		: m_Device(pDevice)
	{
	}

	Resource::Resource(ID3D12Device* pDevice, D3D12_HEAP_PROPERTIES const* pHeapProperties, D3D12_HEAP_FLAGS HeapFlags,
	                   D3D12_RESOURCE_DESC const* pDesc, D3D12_RESOURCE_STATES InitialResourceState,
	                   D3D12_CLEAR_VALUE const* pOptimizedClearValue)
	: m_Device(pDevice)
	{
		THROW_IFF(pDevice->CreateCommittedResource(pHeapProperties, HeapFlags, pDesc, InitialResourceState, pOptimizedClearValue,
			IID_PPV_ARGS(&m_Resource)));
	}

	Resource::Resource(Microsoft::WRL::ComPtr<ID3D12Resource> resource)
		: m_Resource(std::move(resource))
	{
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> & Resource::GetResource()
	{
		return m_Resource;
	}

	ID3D12Resource* Resource::GetResourcePtr() const
	{
		return m_Resource.Get();
	}

	D3D12_GPU_VIRTUAL_ADDRESS Resource::GetGPUVirtualAddress() const
	{
		return m_Resource->GetGPUVirtualAddress();
	}


	void Resource::UpdateBufferResource(ID3D12GraphicsCommandList2* pCmdList, size_t NumElems, size_t ElemSize,
	                                    void const* pBufferData, D3D12_RESOURCE_FLAGS flags)
	{
		ComPtr<ID3D12Resource> intermediateResource;
		Resource::UpdateBufferResource(m_Device, pCmdList, &m_Resource, &intermediateResource, NumElems, ElemSize,
		                               pBufferData, flags);
	}

	void Resource::TransitionAndBarrier(ID3D12GraphicsCommandList2* pGfxCmdList, D3D12_RESOURCE_STATES stateBefore,
		D3D12_RESOURCE_STATES stateAfter) const
	{
		Resource::TransitionAndBarrier(m_Resource.Get(), pGfxCmdList, stateBefore, stateAfter);
	}

	void Resource::UpdateBufferResource(ID3D12Device* pDevice, ID3D12GraphicsCommandList2* pCmdList,
	                                    ID3D12Resource** pDestinationResource,
	                                    ID3D12Resource** pIntermediateResource, size_t NumElems, size_t ElemSize, 
										void const* bufferData, D3D12_RESOURCE_FLAGS flags)
	{
		size_t const bufferSizeInBytes = NumElems * ElemSize;

		auto const destinationHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto const destinationResourceDescription = CD3DX12_RESOURCE_DESC::Buffer(bufferSizeInBytes, flags);

		THROW_IFF(pDevice->CreateCommittedResource(
			&destinationHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&destinationResourceDescription,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(pDestinationResource)));

		if (bufferData)
		{
			auto const intermediateHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			auto const intermediateResourceDescription = CD3DX12_RESOURCE_DESC::Buffer(bufferSizeInBytes);
			THROW_IFF(pDevice->CreateCommittedResource(
				&intermediateHeapProperties,
				D3D12_HEAP_FLAG_NONE,
				&intermediateResourceDescription,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(pIntermediateResource)));
			D3D12_SUBRESOURCE_DATA subresourceData;
			subresourceData.pData = bufferData;
			subresourceData.RowPitch = bufferSizeInBytes;
			subresourceData.SlicePitch = subresourceData.RowPitch;

			UpdateSubresources(pCmdList,
				*pDestinationResource, *pIntermediateResource,
				0, 0, 1, &subresourceData);
		}
	}

	void Resource::UpdateBufferResource(ID3D12Device* pDevice, ID3D12GraphicsCommandList2* pCmdList,
		Resource& DestinationResource,Resource& IntermediateResource, size_t NumElems, size_t ElemSize,
		void const* bufferData, D3D12_RESOURCE_FLAGS flags)
	{
		Resource::UpdateBufferResource(pDevice, pCmdList, &DestinationResource.m_Resource, &IntermediateResource.m_Resource, NumElems, ElemSize,bufferData, flags);
	}

	void Resource::TransitionAndBarrier(ID3D12Resource* pResource, ID3D12GraphicsCommandList2* pGfxCmdList,
	                                    D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
	{
		CD3DX12_RESOURCE_BARRIER const transitionResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			pResource, stateBefore, stateAfter);
		pGfxCmdList->ResourceBarrier(1UL, &transitionResourceBarrier);
	}
}
