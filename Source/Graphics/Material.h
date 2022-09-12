#pragma once
#include <fstream>
#include <sstream>
#include <d3d12.h>
#include <wrl.h>
#include <filesystem>
#include <cstdint>
#include <optional>
#include <algorithm>
#include <yaml-cpp/yaml.h>
#include "RootSignature.h"
#include "Type.h"
#include "../Core/Log.h"
#include "RootSignatureLibrary.h"

namespace acc3d::Graphics
{
	/*
	 *	--------------------------------------Design goals for the material system-----------------------------------------
	 *
	 * - There should be a list of fixed root signatures with well-defined shader parameters / constant buffers;
	 * If a different parameters need in the shaders occur, simply add another fixed root signature to the list.
	 *
	 * - For a PBR material for example, we'd create a class derived from Material class as base, we'd have functions
	 * to control and change diffuse, roughness, metallic, ambient occlusion textures. We should be able to even control the
	 * fresnel function, normal distribution function and so on.
	 *
	 * 
	 */

	/* ATM This is just a general idea/test of what it should look like. */

	class Material
	{
    public:
		explicit Material(std::filesystem::path const& pathToMaterialFile, RootSignatureId rootSignatureId);

		[[nodiscard]] RootSignatureId GetRootSignatureId() const;

        template<typename T>
        void Set32BitConstant(ID3D12GraphicsCommandList2* pGfxCmdList, UINT rootParameterIndex,T
        data,UINT destOffsetIn32BitValues = 0) const;

        void SetGraphicsRootSignature(ID3D12GraphicsCommandList2* pGfxCmdList) const;

	private:
		RootSignatureId m_RootSignatureId = ROOT_SIGNATURE_ID_EMPTY_KEY_VALUE;
	};

    template<typename T>
    void Material::Set32BitConstant(ID3D12GraphicsCommandList2* pGfxCmdList, UINT rootParameterIndex,T
    data,UINT destOffsetIn32BitValues) const
    {
        static_assert(sizeof(T) >= 4, "Material::Set32BitConstants<T>() needs a type that is "
                                      "at least 32 bits.");
        pGfxCmdList->SetGraphicsRoot32BitConstant(rootParameterIndex,data,destOffsetIn32BitValues);
    }

}