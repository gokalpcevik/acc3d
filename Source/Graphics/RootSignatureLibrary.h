#pragma once
#include <filesystem>
#include <sparsehash/dense_hash_map>
#include "Material/RootSignatureFileDeserializer.h"
#include "Wrappers/RootSignature.h"
#include "Type.h"

namespace acc3d::Graphics
{
	class RootSignatureLibrary
	{
	public:
		RootSignatureLibrary() = default;

		static RootSignature const* CreateRootSignatureEntry(
			ID3D12Device* pDevice,
			std::filesystem::path const& rootSigDescFilePath, 
			RootSignatureId id);

		static RootSignature const* CreateRootSignatureEntry(
			ID3D12Device* pDevice,
			RootSignatureInitializer const& init
		);

        static RootSignature const* CreateRootSignatureEntry(
                ID3D12Device* pDevice,
                RootSignatureFileDeserializer const& deserializer,
                RootSignatureId id
                );

		static RootSignature const* CreateRootSignatureEntry(
			ID3D12Device* pDevice,
			ID3DBlob* pRootSignatureBlob, 
			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc,
			RootSignatureId id,
			D3D12_ROOT_SIGNATURE_FLAGS flags);

		static void Init();

		static void Shutdown();

		static bool IsLoaded(RootSignatureId id);

		static void Remove(RootSignatureId id);

		static RootSignature const* Get(RootSignatureId id);

	private:
		static google::dense_hash_map<RootSignatureId, RootSignature const*> s_RootSignatureMap;
	};
	
}