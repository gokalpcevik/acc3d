#include "LightContext.h"

namespace acc3d::Graphics
{
	using Microsoft::WRL::ComPtr;

	void LightContext::Populate(ID3D12Device* pDevice)
	{
		for(size_t i = 0; i < g_NUM_FRAMES_IN_FLIGHT; ++i)
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.NumDescriptors = g_MAX_NUM_OF_DIR_LIGHTS;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			desc.NodeMask = 0;

			ComPtr<ID3D12DescriptorHeap> pDescriptorHeap;

			THROW_IFF(pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pDescriptorHeap)));
			m_LightDescriptorHeaps[i].reset();
			m_LightDescriptorHeaps[i] = std::make_unique<DescriptorHeap>(pDescriptorHeap,D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			auto CBVDescriptorSize = DescriptorHeap::GetDescriptorSizeInfo(pDevice).CBV_SRV_UAVDescriptorSize;

			CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_LightDescriptorHeaps[i]->GetCPUDescriptorHandleForHeapStart());

			for(size_t j = 0; j < g_MAX_NUM_OF_DIR_LIGHTS; ++j)
			{

				CD3DX12_HEAP_PROPERTIES properties(D3D12_HEAP_TYPE_UPLOAD);
				CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(1024ULL * 64ULL, D3D12_RESOURCE_FLAG_NONE);

				ComPtr<ID3D12Resource> pResource;
				THROW_IFF(pDevice->CreateCommittedResource(&properties, D3D12_HEAP_FLAG_NONE, &desc,
					D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pResource)));
				auto& rsc = m_LightResources[i][j];

				rsc.reset();
				rsc = std::make_unique<Resource>(pResource);
				D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};

				cbvDesc.BufferLocation = rsc->GetGPUVirtualAddress();
				cbvDesc.SizeInBytes = sizeof(ECS::DirectionalLightComponent) + (256 - sizeof(ECS::DirectionalLightComponent));

				pDevice->CreateConstantBufferView(&cbvDesc, handle);

				CD3DX12_RANGE readRange(0, 0);
				THROW_IFF(rsc->GetResource()->Map(0, &readRange, reinterpret_cast<void**>(&m_MappedLightEntry[i * g_MAX_NUM_OF_DIR_LIGHTS + j])));
				handle.Offset((UINT)CBVDescriptorSize);
			}
		}
	}

	void LightContext::Clear()
	{
		for(size_t i = 0; i < g_NUM_FRAMES_IN_FLIGHT; ++i)
		{
			for(size_t j = 0; j < g_MAX_NUM_OF_DIR_LIGHTS; ++j)
			{
				m_LightResources[i][j]->GetResource()->Unmap(0, nullptr);
				m_LightResources[i][j].reset(nullptr);
			}
			m_LightDescriptorHeaps[i].reset(nullptr);
		}
	}

	void LightContext::SetLightEntry(ECS::DirectionalLightComponent const& entry, size_t backBufferIndex, size_t lightIndex) const
	{
		std::memcpy(m_MappedLightEntry[backBufferIndex * g_MAX_NUM_OF_DIR_LIGHTS + lightIndex], &entry, sizeof(ECS::DirectionalLightComponent));
	}

	void LightContext::SetLightEntriesDefault(size_t backBufferIndex) const
	{
		ECS::DirectionalLightComponent def{};

		for(size_t i = 0; i < g_MAX_NUM_OF_DIR_LIGHTS; ++i)
		{
			std::memcpy(m_MappedLightEntry[backBufferIndex * g_MAX_NUM_OF_DIR_LIGHTS + i], &def, sizeof(ECS::DirectionalLightComponent));
		}
	}

	DescriptorHeap* LightContext::GetDescriptorHeap(size_t backBufferIndex) const
	{
		return m_LightDescriptorHeaps[backBufferIndex].get();
	}

	Resource* LightContext::GetResource(size_t backBufferIndex, size_t lightIndex) const
	{
		return m_LightResources[backBufferIndex][lightIndex].get();
	}
}
