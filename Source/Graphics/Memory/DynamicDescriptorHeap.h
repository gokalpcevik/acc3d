#pragma once
#include <d3d12.h>

namespace acc3d::Graphics::Memory
{
	class DynamicDescriptorHeap
	{
	public:


	private:
		ID3D12DescriptorHeap* m_DescriptorHeap = nullptr;
	};
}