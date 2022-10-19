#include "DescriptorAllocatorPage.h"

namespace acc3d::Graphics::Memory
{
	DescriptorAllocatorPage::DescriptorAllocatorPage(ID3D12Device* pDevice, uint32_t descriptorsPerPage,
		D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType)
			: m_DescriptorHeapType(descriptorHeapType),
			  m_DescriptorHandleIncrementSize(pDevice->GetDescriptorHandleIncrementSize(descriptorHeapType)),
			  m_NumFreeHandles(descriptorsPerPage),
			  m_DescriptorsPerPage(descriptorsPerPage)
		{
		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descriptorHeapDesc.NumDescriptors = descriptorsPerPage;
		descriptorHeapDesc.Type = descriptorHeapType;
		descriptorHeapDesc.NodeMask = 0;

		THROW_IFF(pDevice->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap)));
		m_BaseCPUDescriptorHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		this->AddNewBlock(0, m_NumFreeHandles);
	}

	DescriptorAllocatorPage::~DescriptorAllocatorPage()
	{
		if (m_DescriptorHeap)
			m_DescriptorHeap->Release();
	}

	bool DescriptorAllocatorPage::HasSpace(size_t numDescriptors) const
	{
		return m_FreeBlocksBySize.lower_bound(numDescriptors) != m_FreeBlocksBySize.end();
	}

	size_t DescriptorAllocatorPage::GetNumFreeHandles() const
	{
		return m_NumFreeHandles;
	}

	DescriptorAllocation DescriptorAllocatorPage::Allocate(OffsetType size)
	{
		std::lock_guard <std::mutex> lock(m_AllocationMutex);

		if (m_NumFreeHandles < size)
			return {};

		auto SmallestBlockItIt = m_FreeBlocksBySize.lower_bound(size);
		if (SmallestBlockItIt == m_FreeBlocksBySize.end())
			return {};

		auto SmallestBlockIt = SmallestBlockItIt->second;
		auto Offset = SmallestBlockIt->first;
		auto NewOffset = Offset + size;
		auto NewSize = SmallestBlockIt->second.Size - size;

		m_FreeBlocksBySize.erase(SmallestBlockItIt);
		m_FreeBlocksByOffset.erase(SmallestBlockIt);

		if(NewSize > 0)
			this->AddNewBlock(NewOffset, NewSize);

		m_NumFreeHandles -= size;
		return DescriptorAllocation{
			CD3DX12_CPU_DESCRIPTOR_HANDLE(m_BaseCPUDescriptorHandle,Offset,m_DescriptorHandleIncrementSize),
			m_DescriptorHeapType,
			static_cast<UINT>(Offset),
			static_cast<UINT>(size) * m_DescriptorHandleIncrementSize,
			static_cast<UINT>(size),
			this};
	}

	void DescriptorAllocatorPage::Free(DescriptorAllocation& allocation, uint64_t frameNumber)
	{
		allocation.SetNull();
		m_StaleDescriptors.emplace(allocation.GetOffsetFromBase(), allocation.GetNumberOfHandles(), frameNumber);
	}

	void DescriptorAllocatorPage::ReleaseStaleDescriptors(uint64_t frameNumber)
	{
		while(!m_StaleDescriptors.empty() && m_StaleDescriptors.front().FrameNumber <= frameNumber)
		{
			auto const& OldestDescriptorAlloc = m_StaleDescriptors.front();

			this->FreeBlock(OldestDescriptorAlloc.Offset, OldestDescriptorAlloc.Size);
			m_StaleDescriptors.pop();
		}
	}

	uint32_t DescriptorAllocatorPage::ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle) const
	{
		return (m_BaseCPUDescriptorHandle.ptr - handle.ptr) / m_DescriptorHandleIncrementSize;
	}

	void DescriptorAllocatorPage::AddNewBlock(OffsetType offset, OffsetType size)
	{
		auto NewBlockIt = m_FreeBlocksByOffset.emplace(offset, size);
		auto OrderIt = m_FreeBlocksBySize.emplace(size, NewBlockIt.first);
		NewBlockIt.first->second.OrderBySizeIt = OrderIt;
	}

	void DescriptorAllocatorPage::FreeBlock(OffsetType offset, OffsetType size)
	{
		// Get the next block whose offset is greater than block's offset that is to be freed.
		auto NextBlockIt = m_FreeBlocksByOffset.upper_bound(offset);
		auto PrevBlockIt = NextBlockIt;

		// If it is not the first block in the list
		if (PrevBlockIt != m_FreeBlocksByOffset.begin())
			// Go to the previous block
			--PrevBlockIt;
		else
			// Otherwise set it to the end of the list to indicate that no block comes before the one being freed.
			PrevBlockIt = m_FreeBlocksByOffset.end();


		OffsetType NewSize, NewOffset;

		if( PrevBlockIt != m_FreeBlocksByOffset.end() && offset == PrevBlockIt->first + PrevBlockIt->second.Size)
		{
			/*
			 * The previous block is exactly behind the block that is being freed.
			 * Previous Block's Offset	     Offset
			 * |							  |
			 * [<-----Previous Block Size---->|<--------Size-------->
			 */
			NewSize = PrevBlockIt->second.Size + size;
			NewOffset = PrevBlockIt->first;

			if(NextBlockIt != m_FreeBlocksByOffset.end() && offset + size == NextBlockIt->first)
			{
				/*
				* The previous block, the block to be freed, and the next block are all aligned.
				* Previous Block's Offset	     Offset				Next Block Offset
				* |								 |						|
				* <-----Previous Block Size----->|<--------Size-------->|<-------Next Block Size------>
				*/

				NewSize += NextBlockIt->second.Size;
				m_FreeBlocksBySize.erase(PrevBlockIt->second.OrderBySizeIt);
				m_FreeBlocksBySize.erase(NextBlockIt->second.OrderBySizeIt);

				++NextBlockIt;
				m_FreeBlocksByOffset.erase(PrevBlockIt, NextBlockIt);
			}
			else
			{
				// PrevBlock.Offset           Offset                       NextBlock.Offset 
				// |                          |                            |
				// |<-----PrevBlock.Size----->|<------Size-------->| ~ ~ ~ |<-----NextBlock.Size----->|
				//

				m_FreeBlocksBySize.erase(PrevBlockIt->second.OrderBySizeIt);
				m_FreeBlocksByOffset.erase(PrevBlockIt);
			}
		}
		else if( NextBlockIt != m_FreeBlocksByOffset.end() && offset + size == NextBlockIt->first)
		{
			// PrevBlock.Offset                   Offset               NextBlock.Offset 
			// |                                  |                    |
			// |<-----PrevBlock.Size----->| ~ ~ ~ |<------Size-------->|<-----NextBlock.Size----->|

			NewSize = size + NextBlockIt->second.Size;
			NewOffset = offset;
			m_FreeBlocksBySize.erase(NextBlockIt->second.OrderBySizeIt);
			m_FreeBlocksByOffset.erase(NextBlockIt);
		}
		else
		{
			// PrevBlock.Offset                   Offset                       NextBlock.Offset 
			// |                                  |                            |
			// |<-----PrevBlock.Size----->| ~ ~ ~ |<------Size-------->| ~ ~ ~ |<-----NextBlock.Size----->|
			//
			NewSize = size;
			NewOffset = offset;
		}

		this->AddNewBlock(NewOffset, NewSize);

		m_NumFreeHandles += size;
	}
}
