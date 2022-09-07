#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include "ResultHandler.h"

namespace acc3d::Graphics
{
	class Resource
	{
	public:
		explicit Resource(ID3D12Device* pDevice);

		explicit Resource(Microsoft::WRL::ComPtr<ID3D12Resource>const & resource);

		// Constructor for committed resources
		Resource(ID3D12Device* pDevice, D3D12_HEAP_PROPERTIES const* pHeapProperties, D3D12_HEAP_FLAGS HeapFlags, 
		         D3D12_RESOURCE_DESC const* pDesc, D3D12_RESOURCE_STATES InitialResourceState, 
		         D3D12_CLEAR_VALUE const* pOptimizedClearValue);

		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource> & GetResource();
		[[nodiscard]] ID3D12Resource* GetResourcePtr() const;

		[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

		void UpdateBufferResource(ID3D12GraphicsCommandList2* pCmdList, size_t NumElems, size_t ElemSize,
		                          void const* pBufferData,
		                          D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);


		void TransitionAndBarrier(ID3D12GraphicsCommandList2* pGfxCmdList, D3D12_RESOURCE_STATES stateBefore,
		                          D3D12_RESOURCE_STATES stateAfter) const;

		static void UpdateBufferResource(ID3D12Device* pDevice, ID3D12GraphicsCommandList2* pCmdList,
		                                 ID3D12Resource** pDestinationResource,
		                                 ID3D12Resource** pIntermediateResource, size_t NumElems, size_t ElemSize, void const* bufferData,
		                                 D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

		static void UpdateBufferResource(ID3D12Device* pDevice, ID3D12GraphicsCommandList2* pCmdList,
		                                Resource& DestinationResource, Resource& IntermediateResource, size_t NumElems,
		                                size_t ElemSize, void const* bufferData,
		                                D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

		static void TransitionAndBarrier(ID3D12Resource* pResource, ID3D12GraphicsCommandList2* pGfxCmdList,
		                       D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource;
		// Non-owning pointer to the D3D12Device 
		ID3D12Device* m_Device = nullptr;
	};
}