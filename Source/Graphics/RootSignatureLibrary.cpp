#include "RootSignatureLibrary.h"

namespace acc3d::Graphics
{
	google::dense_hash_map<RootSignatureId, RootSignature const*> RootSignatureLibrary::s_RootSignatureMap{};

	RootSignature const* RootSignatureLibrary::CreateRootSignatureEntry(
		ID3D12Device* pDevice,
		std::filesystem::path const& rootSigDescFilePath,
		RootSignatureId id)
	{
		if (IsLoaded(id)) return RootSignatureLibrary::Get(id);

		RootSignatureFileDeserializer const deserializer(rootSigDescFilePath);

		if(deserializer)
		{
			auto[pBlob,pErrorBlob] = RootSignature::SerializeVersionedRootSignatureWithHighestVersion(pDevice, deserializer);
			if (pBlob)
			{
				RootSignature const* pRootSig = new RootSignature(pDevice, pBlob->GetBufferPointer(), pBlob->GetBufferSize());
				s_RootSignatureMap[id] = pRootSig;
				return pRootSig;
			}
			else if(pErrorBlob)
			{
				acc3d_error(
					"Root signature blob could not be created correctly for root signature description file {0}.\nError:{1}",
					rootSigDescFilePath.string(), static_cast<char const*>(pErrorBlob->GetBufferPointer()));
				return nullptr;
			}
		}

		return nullptr;
	}

	RootSignature const* RootSignatureLibrary::CreateRootSignatureEntry(ID3D12Device* pDevice,
		RootSignatureInitializer const& init)
	{
		return CreateRootSignatureEntry(pDevice, init.RootSignaturePath, init.RootSignatureId);
	}

	RootSignature const* RootSignatureLibrary::CreateRootSignatureEntry(
		ID3D12Device* pDevice,
		ID3DBlob* pRootSignatureBlob,
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc,
		RootSignatureId id,
		D3D12_ROOT_SIGNATURE_FLAGS flags)
	{
        if(IsLoaded(id)) return Get(id);
		auto [pBlob, pErrorBlob] = RootSignature::SerializeVersionedRootSignatureWithHighestVersion(pDevice,rootSigDesc);
		if (pBlob)
		{
			RootSignature const* pRootSig = new RootSignature(pDevice, pBlob->GetBufferPointer(), pBlob->GetBufferSize());
			s_RootSignatureMap[id] = pRootSig;
			return pRootSig;
		}
		else if(pErrorBlob)
		{
			acc3d_error(
				"Root signature blob could not be created correctly for root signature id {0:X}.\nError:{1}",
				id,static_cast<char const*>(pErrorBlob->GetBufferPointer()));
			return nullptr;
		}
		return nullptr;
	}

    RootSignature const *RootSignatureLibrary::CreateRootSignatureEntry(ID3D12Device *pDevice,
                                                                        const RootSignatureFileDeserializer &deserializer,
                                                                        RootSignatureId id)
    {
        if (IsLoaded(id)) return RootSignatureLibrary::Get(id);

        if(deserializer)
        {
            auto[pBlob,pErrorBlob] = RootSignature::SerializeVersionedRootSignatureWithHighestVersion(pDevice, deserializer);
            if (pBlob)
            {
                RootSignature const* pRootSig = new RootSignature(pDevice, pBlob->GetBufferPointer(), pBlob->GetBufferSize());
                s_RootSignatureMap[id] = pRootSig;
                return pRootSig;
            }
            else if(pErrorBlob)
            {
                acc3d_error(
                        "Root signature blob could not be created correctly for root signature "
                        "description file.\nError:{0}",
                         static_cast<char const*>(pErrorBlob->GetBufferPointer()));
                return nullptr;
            }
        }
        return nullptr;
    }


    void RootSignatureLibrary::Init()
	{
		s_RootSignatureMap.set_empty_key(ROOT_SIGNATURE_ID_EMPTY_KEY_VALUE);
		s_RootSignatureMap.set_deleted_key(ROOT_SIGNATURE_ID_DELETED_KEY_VALUE);
	}

	void RootSignatureLibrary::Shutdown()
	{
		for(std::pair<RootSignatureId,RootSignature const*> const it : s_RootSignatureMap)
		{
			delete it.second;
		}
	}


	bool RootSignatureLibrary::IsLoaded(RootSignatureId id)
	{
		return s_RootSignatureMap.find(id) != s_RootSignatureMap.end();
	}

	void RootSignatureLibrary::Remove(RootSignatureId id)
	{
		s_RootSignatureMap.erase(id);
	}

	RootSignature const* RootSignatureLibrary::Get(RootSignatureId id)
	{
		return s_RootSignatureMap[id];
	}

}
