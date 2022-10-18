#include "Material.h"

namespace acc3d::Graphics
{
	using Microsoft::WRL::ComPtr;

	PBRMaterial::PBRMaterial(PBRMaterialDescription desc)
		: m_Desc(std::move(desc))
	{

	}

	void PBRMaterial::Populate(ID3D12Device* pDevice, D3D12MA::Allocator* pAllocator)
	{
		ComPtr<ID3D12Resource> pDiffuseResource;
		std::unique_ptr<uint8_t[]> decodedData;
		D3D12_SUBRESOURCE_DATA sbData{};
		DirectX::LoadWICTextureFromFile(pDevice, m_Desc.DiffuseTexturePath.c_str(), &pDiffuseResource, decodedData, sbData);
		
	}

	void PBRMaterial::BeforeDraw(OptionalMaterialData const& data)
	{

	}
}
