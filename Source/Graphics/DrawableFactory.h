#pragma once
#include "Type.h"
#include "Drawable.h"
#include "ShaderReflection.h"
#include "ShaderLibrary.h"
#include "Wrappers/CommandList.h"
#include "Wrappers/CommandAllocator.h"
#include "RootSignatureLibrary.h"
#include "../AssetCore/MeshLibrary.h"
#include <D3D12MemAlloc.h>
#include <d3d12.h>
#include <wrl.h>

namespace acc3d::Graphics
{
	class DrawableFactory
	{
	public:
		DrawableFactory() = default;

		static Drawable* CreateDrawable(
			ID3D12Device2* pDevice,
			D3D12MA::Allocator* pAllocator,
			ID3D12CommandQueue* pCopyCommandQueue,
			ID3D12Fence* pCopyFence,
			uint64_t& fenceValue,
			HANDLE fenceEvent,
			Asset::MeshAssetId assetId, 
			RootSignatureInitializer const& rootSigInit);

	};
}