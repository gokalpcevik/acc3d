#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl/client.h>
#include "ResultHandler.h"
#include "Material/RootSignatureFileDeserializer.h"

namespace acc3d::Graphics
{
	class RootSignature
	{
	public:
		RootSignature(ID3D12Device* pDevice,void const* pBlobWithRootSignature, SIZE_T blobLengthInBytes);
		

		static std::pair<Microsoft::WRL::ComPtr<ID3DBlob>, Microsoft::WRL::ComPtr<ID3DBlob>>
		SerializeVersionedRootSignatureWithHighestVersion(ID3D12Device* pDevice,
		                                                  CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC const&
		                                                  rootSignatureDescription);

		static std::pair<Microsoft::WRL::ComPtr<ID3DBlob>, Microsoft::WRL::ComPtr<ID3DBlob>>
			SerializeVersionedRootSignatureWithHighestVersion(ID3D12Device* pDevice,
				RootSignatureFileDeserializer const& fileDeserializer);

		[[nodiscard]] ID3D12RootSignature* GetD3D12RootSignaturePtr() const;
		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetD3D12RootSignature();
	private:

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
	};
}