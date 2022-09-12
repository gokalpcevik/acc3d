#include "Material.h"

namespace acc3d::Graphics
{
	RootSignatureId Material::GetRootSignatureId() const
	{
		return m_RootSignatureId;
	}

    void Material::SetGraphicsRootSignature(ID3D12GraphicsCommandList2 *pGfxCmdList) const
    {
        ID3D12RootSignature* pRootSignature = RootSignatureLibrary::Get(this->GetRootSignatureId())->GetD3D12RootSignaturePtr();
        pGfxCmdList->SetGraphicsRootSignature(pRootSignature);
    }
}
