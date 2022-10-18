#pragma once
#include <d3d12.h>
#include <D3D12MemAlloc.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <filesystem>
#include <cstdint>
#include <directxtk12/WICTextureLoader.h>
#include <directxtk12/DDSTextureLoader.h>

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
	 */

	/*
	 * ATM This is just a general idea/test of what it should look like.
	 */

	struct OptionalMaterialData
	{
		DirectX::XMMATRIX Mvp;
		DirectX::XMMATRIX Model;
		DirectX::XMVECTOR CameraPosition;
	};

	class Material
	{
	public:
		Material() = default;
		virtual ~Material() = default;
		virtual void Populate(ID3D12Device* pDevice, D3D12MA::Allocator* pAllocator) = 0;

		virtual void BeforeDraw(OptionalMaterialData const& data) = 0;
	};

	struct PBRMaterialDescription
	{
		std::filesystem::path DiffuseTexturePath{};
		std::filesystem::path MetallicTexturePath{};
		std::filesystem::path RoughnessTexturePath{};
		std::filesystem::path AmbientOcclusionTexturePath{};
	};

	class PBRMaterial : public Material
	{
	public:
		explicit PBRMaterial(PBRMaterialDescription desc);

		void Populate(ID3D12Device* pDevice, D3D12MA::Allocator* pAllocator) override;
		void BeforeDraw(OptionalMaterialData const& data) override;
	private:
		PBRMaterialDescription m_Desc;
	};
}
