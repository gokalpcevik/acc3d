#include "DescriptorAllocator.h"

namespace acc3d::Graphics::Memory
{
	DescriptorAllocator::DescriptorAllocator(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type,
		uint32_t descriptorsPerPage)
			:
	m_Device(pDevice),
	m_DescriptorHeapType(type),
	m_DescriptorsPerPage(descriptorsPerPage)
	{
		m_PagePool.emplace_back(this->CreateDescriptorAllocatorPage());
	}

	DescriptorAllocation DescriptorAllocator::Allocate(OffsetType numDescriptors)
	{
		for(auto const& page : m_PagePool)
		{
			if(page->HasSpace(numDescriptors))
				return page->Allocate(numDescriptors);
			else if(numDescriptors <= m_DescriptorsPerPage)
			{
				auto const & newPage = m_PagePool.emplace_back(this->CreateDescriptorAllocatorPage());
				return newPage->Allocate(numDescriptors);
			}
			else
			{
				acc3d_error("Descriptor Allocator could not satisfy allocation request because the requested allocation size exceeds the number of descriptors per page.");
				return DescriptorAllocation();
			}
		}

		auto const& newPage = m_PagePool.emplace_back(this->CreateDescriptorAllocatorPage());
		return newPage->Allocate(numDescriptors);
	}

	void DescriptorAllocator::Free(DescriptorAllocation& allocation, uint64_t frameNumber)
	{
		if(DescriptorAllocatorPage * const page = allocation.GetOwningPage())
		{
			page->Free(allocation, frameNumber);
		}
	}

	void DescriptorAllocator::ReleaseStaleDescriptors(uint64_t frameNumber) const
	{
		for(auto const& page : m_PagePool)
		{
			page->ReleaseStaleDescriptors(frameNumber);
		}
	}

	std::shared_ptr<DescriptorAllocatorPage> DescriptorAllocator::CreateDescriptorAllocatorPage()
	{
		return std::make_shared<DescriptorAllocatorPage>(m_Device, m_DescriptorsPerPage, m_DescriptorHeapType);
	}
}
