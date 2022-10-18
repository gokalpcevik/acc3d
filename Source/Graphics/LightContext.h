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

namespace acc3d::Graphics
{
	class LightContext
	{
	public:
		LightContext() = default;

		void Populate(ID3D12Device* pDevice, D3D12MA::Allocator* pAllocator);

		void Release();

		void SetLightEntry(ECS::DirectionalLightComponent const& entry, size_t backBufferIndex, size_t lightIndex) const;

		void SetLightEntriesDefault(size_t backBufferIndex) const;

		[[nodiscard]] DescriptorHeap* GetDescriptorHeap(size_t backBufferIndex) const;
		[[nodiscard]] ID3D12Resource* GetResource(size_t backBufferIndex, size_t lightIndex) const;
	public:
		std::unique_ptr<DescriptorHeap> m_LightDescriptorHeaps[g_NUM_FRAMES_IN_FLIGHT];
		D3D12MA::Allocation* m_LightAllocations[g_NUM_FRAMES_IN_FLIGHT][g_MAX_NUM_OF_DIR_LIGHTS];
		ECS::DirectionalLightComponent* m_MappedLightEntry[g_NUM_FRAMES_IN_FLIGHT * g_MAX_NUM_OF_DIR_LIGHTS];
	};
}