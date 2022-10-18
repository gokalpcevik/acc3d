#pragma once
#include <d3d12.h>
#include "DescriptorAllocatorPage.h"
#include "../../Core/Log.h"

namespace acc3d::Graphics::Memory
{
	class DescriptorAllocator
	{
		using OffsetType = DescriptorAllocatorPage::OffsetType;
	public:
		DescriptorAllocator(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t descriptorsPerPage = 256);


		DescriptorAllocation Allocate(OffsetType numDescriptors);


		static void Free(DescriptorAllocation& allocation, uint64_t frameNumber);

		void ReleaseStaleDescriptors(uint64_t frameNumber) const;
	private:
		std::shared_ptr<DescriptorAllocatorPage> CreateDescriptorAllocatorPage();

	private:
		ID3D12Device* m_Device = nullptr;
		std::vector< std::shared_ptr<DescriptorAllocatorPage> > m_PagePool;
		D3D12_DESCRIPTOR_HEAP_TYPE m_DescriptorHeapType;
		uint32_t m_DescriptorsPerPage;
	};
}