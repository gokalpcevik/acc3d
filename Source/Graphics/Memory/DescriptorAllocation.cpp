#include "DescriptorAllocation.h"
#include "../../Core/Application.h"
#include "DescriptorAllocatorPage.h"

namespace acc3d::Graphics::Memory
{
	DescriptorAllocation::DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE handle,
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		UINT offsetFromBase,
		UINT sizeInBytes,
		UINT numHandles,
		DescriptorAllocatorPage* pOwningPage)
			:
	m_BaseCPUDescriptorHandle(handle),
	m_DescriptorHeapType(type),
	m_OffsetFromBase(offsetFromBase),
	m_NumHandles(numHandles),
	m_SizeInBytes(sizeInBytes),
	m_OwningPage(pOwningPage)
	{

	}

	void DescriptorAllocation::SetNull()
	{
		m_BaseCPUDescriptorHandle = {};
		m_SizeInBytes = 0;
		m_NumHandles = 0;
		m_OffsetFromBase = 0;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetCPUDescriptorHandle(SIZE_T offset) const
	{
		SIZE_T const IncrementSize = m_SizeInBytes / m_NumHandles;
		return { GetBaseCPUDescriptorHandle().ptr + IncrementSize * offset };
	}
}
