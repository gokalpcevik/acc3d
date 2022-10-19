#pragma once
#include <d3d12.h>

namespace acc3d::Core
{
	class Application;
}

namespace acc3d::Graphics::Memory
{
	class DescriptorAllocatorPage;
	/**
	 * \brief Represents a descriptor allocation residing in a used block in a page in a DescriptorAllocator.
	 */
	class DescriptorAllocation
	{
	public:
		DescriptorAllocation() = default;
		DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE handle,
			D3D12_DESCRIPTOR_HEAP_TYPE type,
			UINT offsetFromBase,
			UINT sizeInBytes,
			UINT numHandles,
			DescriptorAllocatorPage* pOwningPage);
		~DescriptorAllocation() = default;

		void SetNull();
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetBaseCPUDescriptorHandle() const { return m_BaseCPUDescriptorHandle; }
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(SIZE_T offset) const;
		[[nodiscard]] size_t GetOffsetFromBase() const { return m_OffsetFromBase; }
		[[nodiscard]] size_t GetSizeInBytes() const { return m_SizeInBytes; }
		[[nodiscard]] size_t GetNumberOfHandles() const { return m_NumHandles; }
		[[nodiscard]] DescriptorAllocatorPage* GetOwningPage() const { return m_OwningPage; }
	private:
		D3D12_CPU_DESCRIPTOR_HANDLE m_BaseCPUDescriptorHandle{};
		D3D12_DESCRIPTOR_HEAP_TYPE m_DescriptorHeapType{};
		UINT m_OffsetFromBase = 0;
		UINT m_SizeInBytes = 0;
		UINT m_NumHandles = 0;
		DescriptorAllocatorPage* m_OwningPage = nullptr;
	};

}