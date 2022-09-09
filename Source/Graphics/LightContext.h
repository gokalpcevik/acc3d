#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "ResultHandler.h"
#include "Resource.h"
#include "DescriptorHeap.h"
#include "../ECS/LightComponents.h"
#include "Type.h"
#include "RendererConfiguration.h"

namespace acc3d::Graphics
{
	struct LightEntry
	{
		ECS::DirectionalLightComponent DirectionalLight{};
	private:
		float volatile r_padding0[(256 - sizeof(ECS::DirectionalLightComponent)) / sizeof(float)];
	};

	class LightContext
	{
	public:
		LightContext() = default;

		/*
		 * void UploadLightEntry(LightEntry const& entry,size_t index);
		 */
		void Populate(ID3D12Device* pDevice);

		void Clear();

		void SetLightEntry(LightEntry const& entry, size_t backBufferIndex,size_t lightIndex) const;

		[[nodiscard]] DescriptorHeap* GetDescriptorHeap(size_t backBufferIndex) const;
		[[nodiscard]] Resource* GetResource(size_t backBufferIndex, size_t lightIndex) const;
	private:
		std::unique_ptr<DescriptorHeap> m_LightDescriptorHeaps[g_NUM_FRAMES_IN_FLIGHT];
		std::unique_ptr<Resource> m_LightResources[g_NUM_FRAMES_IN_FLIGHT][g_MAX_NUM_OF_DIR_LIGHTS];
		LightEntry* m_MappedLightEntry[g_NUM_FRAMES_IN_FLIGHT][g_MAX_NUM_OF_DIR_LIGHTS];
	};
}