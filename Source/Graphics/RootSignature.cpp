#include "RootSignature.h"

namespace acc3d::Graphics
{
	using Microsoft::WRL::ComPtr;

	RootSignature::RootSignature(ID3D12Device* pDevice, void const* pBlobWithRootSignature, SIZE_T blobLengthInBytes)
	{
		THROW_IFF(pDevice->CreateRootSignature(0, pBlobWithRootSignature, blobLengthInBytes, IID_PPV_ARGS(&m_RootSignature)));
	}

	std::pair<Microsoft::WRL::ComPtr<ID3DBlob>, Microsoft::WRL::ComPtr<ID3DBlob>> RootSignature::
	SerializeVersionedRootSignatureWithHighestVersion(ID3D12Device* pDevice, CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC const& rootSignatureDescription)
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if(FAILED(pDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		ComPtr<ID3DBlob> rootSignatureBlob;
		ComPtr<ID3DBlob> errorBlob;

		THROW_IFF(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription, 
			featureData.HighestVersion, 
			&rootSignatureBlob,
			&errorBlob));
		return { std::move(rootSignatureBlob),std::move(errorBlob) };
	}
}
