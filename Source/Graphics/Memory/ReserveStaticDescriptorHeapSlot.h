#pragma once
#include <d3d12.h>

namespace acc3d::Graphics::Memory
{
	template<size_t Slot,size_t Size, D3D12_DESCRIPTOR_HEAP_TYPE HeapType>
	class ReserveStaticDescriptorHeapSlot
	{
	public:
		static constexpr size_t s_Slot = Slot;
		static constexpr size_t s_Size = Size;
		static constexpr size_t s_HeapType = HeapType;
	};
}