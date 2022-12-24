#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <D3D12MemAlloc.h>
#include "ResultHandler.h"
#include "Wrappers/DescriptorHeap.h"
#include "../ECS/LightComponents.h"
#include "Type.h"
#include "RendererConfiguration.h"

#include "Memory/ReserveStaticDescriptorHeapSlot.h"
#include "Memory/DescriptorAllocation.h"
#include "Memory/DescriptorAllocator.h"

namespace acc3d::Graphics
{
	class LightContext
	: public Memory::ReserveStaticDescriptorHeapSlot<
	g_LIGHT_CONTEXT_STATIC_DESCRIPTOR_HEAP_SLOT,
	g_MAX_NUM_OF_DIR_LIGHTS,
	D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>
	{
	public:
		LightContext() = default;

		void Populate(ID3D12Device* pDevice, D3D12MA::Allocator* pAllocator);
		void Populate(ID3D12Device* pDevice, D3D12MA::Allocator* pAllocator,Memory::DescriptorAllocator* pDescAllocator);

		void Release();
		void Release(uint64_t frameNumber);

		void SetLightEntry(ECS::DirectionalLightComponent const& entry, size_t backBufferIndex, size_t lightIndex) const;

		void SetLightEntriesDefault(size_t backBufferIndex) const;

		[[nodiscard]] DescriptorHeap* GetDescriptorHeap(size_t backBufferIndex) const;
		[[nodiscard]] Memory::DescriptorAllocation GetLightDescriptorAllocation() const;
		[[nodiscard]] ID3D12Resource* GetResource(size_t backBufferIndex, size_t lightIndex) const;
	public:
		// should remove this soon
		std::unique_ptr<DescriptorHeap> m_LightDescriptorHeaps[g_NUM_FRAMES_IN_FLIGHT];
		D3D12MA::Allocation* m_LightBufferResourceAllocations[g_NUM_FRAMES_IN_FLIGHT][g_MAX_NUM_OF_DIR_LIGHTS]{};
		ECS::DirectionalLightComponent* m_MappedLightEntry[g_NUM_FRAMES_IN_FLIGHT * g_MAX_NUM_OF_DIR_LIGHTS]{};
		Memory::DescriptorAllocation m_LightDescriptorAllocation{};
	};
}