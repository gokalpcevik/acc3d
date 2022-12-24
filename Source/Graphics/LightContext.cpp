#include "LightContext.h"

namespace acc3d::Graphics
{
	using Microsoft::WRL::ComPtr;

	void LightContext::Populate(ID3D12Device* pDevice, D3D12MA::Allocator* pAllocator)
	{
		for (size_t i = 0; i < g_NUM_FRAMES_IN_FLIGHT; ++i)
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.NumDescriptors = g_MAX_NUM_OF_DIR_LIGHTS;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			desc.NodeMask = 0;

			ComPtr<ID3D12DescriptorHeap> pDescriptorHeap;

			THROW_IFF(pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pDescriptorHeap)));
			m_LightDescriptorHeaps[i].reset();
			m_LightDescriptorHeaps[i] = std::make_unique<DescriptorHeap>(pDescriptorHeap, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			auto const CBVDescriptorSize = DescriptorHeap::GetDescriptorSizeInfo(pDevice).CBV_SRV_UAVDescriptorSize;

			CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_LightDescriptorHeaps[i]->GetCPUDescriptorHandleForHeapStart());
			for (size_t j = 0; j < g_MAX_NUM_OF_DIR_LIGHTS; ++j)
			{
				CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
				CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(1024ULL * 64ULL, D3D12_RESOURCE_FLAG_NONE);
				D3D12MA::ALLOCATION_DESC allocationDesc{};
				allocationDesc.CustomPool = nullptr;
				allocationDesc.ExtraHeapFlags = D3D12_HEAP_FLAG_NONE;
				allocationDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
				allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

				THROW_IFF(pAllocator->CreateResource(&allocationDesc, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
					&m_LightBufferResourceAllocations[i][j], IID_NULL, NULL));

				ID3D12Resource* pResource = m_LightBufferResourceAllocations[i][j]->GetResource();

				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};

				cbvDesc.BufferLocation = pResource->GetGPUVirtualAddress();
				static_assert(sizeof(ECS::DirectionalLightComponent) <= 256);
				cbvDesc.SizeInBytes = sizeof(ECS::DirectionalLightComponent) + (256 - sizeof(ECS::DirectionalLightComponent));

				pDevice->CreateConstantBufferView(&cbvDesc, handle);
				CD3DX12_RANGE readRange(0, 0);
				THROW_IFF(pResource->Map(0, &readRange, reinterpret_cast<void**>(&m_MappedLightEntry[i * g_MAX_NUM_OF_DIR_LIGHTS + j])));
				handle.Offset((UINT)CBVDescriptorSize);
			}
		}
	}

	void LightContext::Populate(ID3D12Device* pDevice, D3D12MA::Allocator* pAllocator,
		Memory::DescriptorAllocator* pDescAllocator)
	{
		for (size_t i = 0; i < g_NUM_FRAMES_IN_FLIGHT; ++i)
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.NumDescriptors = g_MAX_NUM_OF_DIR_LIGHTS;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			desc.NodeMask = 0;

			ComPtr<ID3D12DescriptorHeap> pDescriptorHeap;

			THROW_IFF(pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pDescriptorHeap)));
			m_LightDescriptorHeaps[i].reset();
			m_LightDescriptorHeaps[i] = std::make_unique<DescriptorHeap>(pDescriptorHeap, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			m_LightDescriptorAllocation = pDescAllocator->Allocate(g_MAX_NUM_OF_DIR_LIGHTS);

			auto const CBVDescriptorSize = DescriptorHeap::GetDescriptorSizeInfo(pDevice).CBV_SRV_UAVDescriptorSize;

			CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_LightDescriptorAllocation.GetBaseCPUDescriptorHandle());
			for (size_t j = 0; j < g_MAX_NUM_OF_DIR_LIGHTS; ++j)
			{
				CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
				CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(1024ULL * 64ULL, D3D12_RESOURCE_FLAG_NONE);
				D3D12MA::ALLOCATION_DESC allocationDesc{};
				allocationDesc.CustomPool = nullptr;
				allocationDesc.ExtraHeapFlags = D3D12_HEAP_FLAG_NONE;
				allocationDesc.Flags = D3D12MA::ALLOCATION_FLAG_NONE;
				allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

				THROW_IFF(pAllocator->CreateResource(&allocationDesc, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
					&m_LightBufferResourceAllocations[i][j], IID_NULL, NULL));

				ID3D12Resource* pResource = m_LightBufferResourceAllocations[i][j]->GetResource();

				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};

				cbvDesc.BufferLocation = pResource->GetGPUVirtualAddress();
				static_assert(sizeof(ECS::DirectionalLightComponent) <= 256);
				cbvDesc.SizeInBytes = sizeof(ECS::DirectionalLightComponent) + (256 - sizeof(ECS::DirectionalLightComponent));

				pDevice->CreateConstantBufferView(&cbvDesc, handle);
				CD3DX12_RANGE readRange(0, 0);
				THROW_IFF(pResource->Map(0, &readRange, reinterpret_cast<void**>(&m_MappedLightEntry[i * g_MAX_NUM_OF_DIR_LIGHTS + j])));
				handle.Offset((UINT)CBVDescriptorSize);
			}
		}
	}

	void LightContext::Release()
	{
		for (size_t i = 0; i < g_NUM_FRAMES_IN_FLIGHT; ++i)
		{
			for (size_t j = 0; j < g_MAX_NUM_OF_DIR_LIGHTS; ++j)
			{
				m_LightBufferResourceAllocations[i][j]->GetResource()->Unmap(0, nullptr);
				m_LightBufferResourceAllocations[i][j]->Release();
			}
			
		}
	}

	void LightContext::Release(uint64_t frameNumber)
	{
		for (size_t i = 0; i < g_NUM_FRAMES_IN_FLIGHT; ++i)
		{
			for (size_t j = 0; j < g_MAX_NUM_OF_DIR_LIGHTS; ++j)
			{
				m_LightBufferResourceAllocations[i][j]->GetResource()->Unmap(0, nullptr);
				m_LightBufferResourceAllocations[i][j]->Release();
			}
		}
		Memory::DescriptorAllocator::Free(m_LightDescriptorAllocation, frameNumber);
	}

	void LightContext::SetLightEntry(ECS::DirectionalLightComponent const& entry, size_t backBufferIndex, size_t lightIndex) const
	{
		std::memcpy(m_MappedLightEntry[backBufferIndex * g_MAX_NUM_OF_DIR_LIGHTS + lightIndex], &entry, sizeof(ECS::DirectionalLightComponent));
	}

	void LightContext::SetLightEntriesDefault(size_t backBufferIndex) const
	{
		const ECS::DirectionalLightComponent def{};

		for (size_t i = 0; i < g_MAX_NUM_OF_DIR_LIGHTS; ++i)
		{
			std::memcpy(m_MappedLightEntry[backBufferIndex * g_MAX_NUM_OF_DIR_LIGHTS + i], &def, sizeof(ECS::DirectionalLightComponent));
		}
	}

	DescriptorHeap* LightContext::GetDescriptorHeap(size_t backBufferIndex) const
	{
		return m_LightDescriptorHeaps[backBufferIndex].get();
	}

	Memory::DescriptorAllocation LightContext::GetLightDescriptorAllocation() const
	{
		return m_LightDescriptorAllocation;
	}

	ID3D12Resource* LightContext::GetResource(size_t backBufferIndex, size_t lightIndex) const
	{
		return m_LightBufferResourceAllocations[backBufferIndex][lightIndex]->GetResource();
	}
}
