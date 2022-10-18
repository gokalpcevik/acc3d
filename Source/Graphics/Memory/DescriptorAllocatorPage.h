#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <cstdint>
#include <map>
#include <queue>
#include "DescriptorAllocation.h"
#include "../ResultHandler.h"

namespace acc3d::Graphics::Memory
{
	class DescriptorAllocatorPage
	{
	public:
		using OffsetType = size_t;

		DescriptorAllocatorPage(ID3D12Device* pDevice,
			uint32_t descriptorsPerPage,
			D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType);
		virtual ~DescriptorAllocatorPage();

		[[nodiscard]] D3D12_DESCRIPTOR_HEAP_TYPE GetDescriptorHeapType() const { return m_DescriptorHeapType; }
		[[nodiscard]] uint32_t GetDescriptorsPerPage() const { return m_DescriptorsPerPage; }
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetBaseCPUDescriptorHandle() const { return m_BaseCPUDescriptorHandle; }

		[[nodiscard]] bool HasSpace(size_t numDescriptors) const;
		/**
		 * \brief NOTE: Due to fragmentation, allocations equal to, or less than the number of free handles
		 * can still fail. Use DescriptorAllocatorPage::HasSpace to see if the allocation request can be satisfied
		 * before calling DescriptorAllocatorPage::Allocate().
		 * \return Number of free handles in the page.
		 */
		[[nodiscard]] size_t GetNumFreeHandles() const;

		/**
		 * \brief NOTE: Due to fragmentation, allocations equal to, or less than the number of free handles can still fail.
		 * \param size Number of descriptors to allocate.
		 * \return First allocated descriptor.
		 */
		DescriptorAllocation Allocate(OffsetType size = 1);

		/**
		 * \brief Returns an allocation back to the descriptor heap.
		 * \param allocation Allocation to be returned to the heap.
		 * \param frameNumber Frame number or the fence value that should be waited.
		 */
		void Free(DescriptorAllocation& allocation, uint64_t frameNumber);

		/**
		 * \brief Returns any free'd descriptors back to the descriptor heap.
		 */
		void ReleaseStaleDescriptors(uint64_t frameNumber);
	private:
		uint32_t ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle) const;

		void AddNewBlock(OffsetType offset, OffsetType size);

		void FreeBlock(OffsetType offset, OffsetType size);
	private:
		struct FreeBlockInfo;

		using TFreeListByOffsetMap = std::map<OffsetType, FreeBlockInfo>;
		using TFreeListBySizeMap = std::multimap<OffsetType, TFreeListByOffsetMap::iterator>;

		struct FreeBlockInfo
		{
			FreeBlockInfo(OffsetType size) : Size(size) {}

			OffsetType Size;
			// Iterator referencing this block in the multimap sorted by block size.
			TFreeListBySizeMap::iterator OrderBySizeIt;
		};

		struct StaleDescriptorInfo
		{
			StaleDescriptorInfo() = default;
			StaleDescriptorInfo(OffsetType offset, OffsetType size, uint64_t frameNumber)
				: Offset(offset), Size(size), FrameNumber(frameNumber) {}

			OffsetType Offset;
			OffsetType Size;
			uint64_t FrameNumber;
		};

		ID3D12DescriptorHeap* m_DescriptorHeap = nullptr;
		D3D12_DESCRIPTOR_HEAP_TYPE m_DescriptorHeapType{};
		D3D12_CPU_DESCRIPTOR_HANDLE m_BaseCPUDescriptorHandle{};
		UINT m_DescriptorHandleIncrementSize{};

		uint32_t m_NumFreeHandles;
		uint32_t m_DescriptorsPerPage = 0;

		TFreeListByOffsetMap m_FreeBlocksByOffset;
		TFreeListBySizeMap m_FreeBlocksBySize;

		std::queue <StaleDescriptorInfo> m_StaleDescriptors;

		std::mutex m_AllocationMutex;
	};


}