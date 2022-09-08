#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <vector>
#include "ShaderLibrary.h"
#include "ResultHandler.h"
#include "Type.h"

namespace acc3d::Graphics
{
	enum class ShaderDataFormat : uint32_t
	{
		Unknown = 0,
		UInt32_1 = 1,
		UInt32_2 = 2,
		UInt32_3 = 3,
		UInt32_4 = 4,
		SInt32_1 = 5,
		SInt32_2 = 6,
		SInt32_3 = 7,
		SInt32_4 = 8,
		Float32_1 = 9,
		Float32_2 = 10,
		Float32_3 = 11,
		Float32_4 = 12,
	};


	struct VertexLayoutElement
	{
		ShaderDataFormat Format;
		std::string SemanticName;
		uint64_t SemanticIndex;
		uint64_t Register;
		uint64_t Size;
		uint64_t AlignedByteOffset;
	};

	class ShaderDataFormatHelper
	{
	public:
		ShaderDataFormatHelper() = default;

		static DXGI_FORMAT ShaderDataFormatToDXGIFormat(ShaderDataFormat format);
		static uint64_t GetShaderDataFormatSizeInBytes(ShaderDataFormat format);
	};


	class VertexLayout
	{
	public:
		explicit VertexLayout(ShaderId id);

		[[nodiscard]] uint64_t GetNumInputParameters() const { return m_NumInputParameters;  }
		[[nodiscard]] uint64_t GetStride() const { return m_Stride; }
		[[nodiscard]] std::vector<VertexLayoutElement> const& GetElements() const { return m_LayoutElements; }

		std::vector<D3D12_INPUT_ELEMENT_DESC> GetD3D12InputLayout() const;
	private:
		std::vector<VertexLayoutElement> m_LayoutElements;
		uint64_t m_NumInputParameters = 0;
		uint64_t m_Stride = 0;
	};
}
