#include "ShaderReflection.h"

namespace acc3d::Graphics
{
	using Microsoft::WRL::ComPtr;

	std::string_view ShaderDataFormatHelper::ShaderDataFormatToString(ShaderDataFormat format)
	{
		switch (format)
		{
		default: return "DXGI_FORMAT_UNKNOWN";
		case ShaderDataFormat::Unknown: return "DXGI_FORMAT_UNKNOWN";
		case ShaderDataFormat::Float32_1: return "DXGI_FORMAT_R32_FLOAT";
		case ShaderDataFormat::Float32_2: return "DXGI_FORMAT_R32G32_FLOAT";
		case ShaderDataFormat::Float32_3: return "DXGI_FORMAT_R32G32B32_FLOAT";
		case ShaderDataFormat::Float32_4: return "DXGI_FORMAT_R32G32B32A32_FLOAT";
		case ShaderDataFormat::UInt32_1: return "DXGI_FORMAT_R32_UINT";
		case ShaderDataFormat::UInt32_2: return "DXGI_FORMAT_R32G32_UINT";
		case ShaderDataFormat::UInt32_3: return "DXGI_FORMAT_R32G32B32_UINT";
		case ShaderDataFormat::UInt32_4: return "DXGI_FORMAT_R32G32B32A32_UINT";
		case ShaderDataFormat::SInt32_1: return "DXGI_FORMAT_R32_SINT";
		case ShaderDataFormat::SInt32_2: return "DXGI_FORMAT_R32G32_SINT";
		case ShaderDataFormat::SInt32_3: return "DXGI_FORMAT_R32G32B32_SINT";
		case ShaderDataFormat::SInt32_4: return "DXGI_FORMAT_R32G32B32A32_SINT";
		}
	}

	DXGI_FORMAT ShaderDataFormatHelper::ShaderDataFormatToDXGIFormat(ShaderDataFormat format)
	{
		switch (format)
		{
		default: return DXGI_FORMAT_UNKNOWN;
		case ShaderDataFormat::Unknown: return DXGI_FORMAT_UNKNOWN;
		case ShaderDataFormat::Float32_1: return DXGI_FORMAT_R32_FLOAT;
		case ShaderDataFormat::Float32_2: return DXGI_FORMAT_R32G32_FLOAT;
		case ShaderDataFormat::Float32_3: return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataFormat::Float32_4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ShaderDataFormat::UInt32_1: return DXGI_FORMAT_R32_UINT;
		case ShaderDataFormat::UInt32_2: return DXGI_FORMAT_R32G32_UINT;
		case ShaderDataFormat::UInt32_3: return DXGI_FORMAT_R32G32B32_UINT;
		case ShaderDataFormat::UInt32_4: return DXGI_FORMAT_R32G32B32A32_UINT;
		case ShaderDataFormat::SInt32_1: return DXGI_FORMAT_R32_SINT;
		case ShaderDataFormat::SInt32_2: return DXGI_FORMAT_R32G32_SINT;
		case ShaderDataFormat::SInt32_3: return DXGI_FORMAT_R32G32B32_SINT;
		case ShaderDataFormat::SInt32_4: return DXGI_FORMAT_R32G32B32A32_SINT;
		}
	}

	uint64_t ShaderDataFormatHelper::GetShaderDataFormatSizeInBytes(ShaderDataFormat format)
	{
		switch (format)
		{
		default: return 0ULL;
		case ShaderDataFormat::Unknown: return 0ULL;
		case ShaderDataFormat::Float32_1: return 4ULL * 1ULL;
		case ShaderDataFormat::Float32_2: return 4ULL * 2ULL;
		case ShaderDataFormat::Float32_3: return 4ULL * 3ULL;
		case ShaderDataFormat::Float32_4: return 4ULL * 4ULL;
		case ShaderDataFormat::UInt32_1: return 4ULL * 1ULL;
		case ShaderDataFormat::UInt32_2: return 4ULL * 2ULL;
		case ShaderDataFormat::UInt32_3: return 4ULL * 3ULL;
		case ShaderDataFormat::UInt32_4: return 4ULL * 4ULL;
		case ShaderDataFormat::SInt32_1: return 4ULL * 1ULL;
		case ShaderDataFormat::SInt32_2: return 4ULL * 2ULL;
		case ShaderDataFormat::SInt32_3: return 4ULL * 3ULL;
		case ShaderDataFormat::SInt32_4: return 4ULL * 4ULL;
		}
	}

	VertexLayout::VertexLayout(ShaderId id)
	{
		assert(ShaderLibrary::IsLoaded(id)
			&& R"(The shader that is to be reflected for the vertex layout is 
					not loaded into the shader library.)");

		auto& entry = ShaderLibrary::GetCompilationEntry(id);
		assert(entry.ShaderType == ShaderType::Vertex
			&& "Shader needs to be a vertex shader to be reflected for the vertex layout deduction.");

		ComPtr<ID3D12ShaderReflection> pReflection;

		THROW_IFF(D3DReflect(entry.Blob->GetBufferPointer(), entry.Blob->GetBufferSize(), IID_PPV_ARGS(&pReflection)));

		D3D12_SHADER_DESC shaderDesc;
		THROW_IFF(pReflection->GetDesc(&shaderDesc));

		m_NumInputParameters = shaderDesc.InputParameters;

		uint64_t alignedByteOffset = 0;

		for (size_t i = 0; i < m_NumInputParameters; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC sigParamDesc;
			THROW_IFF(pReflection->GetInputParameterDesc(i, &sigParamDesc));

			VertexLayoutElement layoutElement{};

			if (sigParamDesc.Mask == 1)
			{
				if (sigParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				{
					layoutElement.Format = ShaderDataFormat::UInt32_1;
				}
				else if (sigParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				{
					layoutElement.Format = ShaderDataFormat::SInt32_1;
				}
				else if (sigParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					layoutElement.Format = ShaderDataFormat::Float32_1;
				}
			}
			else if (sigParamDesc.Mask <= 3)
			{
				if (sigParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				{
					layoutElement.Format = ShaderDataFormat::UInt32_2;
				}
				else if (sigParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				{
					layoutElement.Format = ShaderDataFormat::SInt32_2;
				}
				else if (sigParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					layoutElement.Format = ShaderDataFormat::Float32_2;
				}
			}
			else if (sigParamDesc.Mask <= 7)
			{
				if (sigParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				{
					layoutElement.Format = ShaderDataFormat::UInt32_3;
				}
				else if (sigParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				{
					layoutElement.Format = ShaderDataFormat::SInt32_3;
				}
				else if (sigParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					layoutElement.Format = ShaderDataFormat::Float32_3;
				}
			}
			else if (sigParamDesc.Mask <= 15)
			{
				if (sigParamDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				{
					layoutElement.Format = ShaderDataFormat::UInt32_4;
				}
				else if (sigParamDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				{
					layoutElement.Format = ShaderDataFormat::UInt32_4;
				}
				else if (sigParamDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					layoutElement.Format = ShaderDataFormat::UInt32_4;
				}
			}
			layoutElement.AlignedByteOffset = alignedByteOffset;
			layoutElement.Size = ShaderDataFormatHelper::GetShaderDataFormatSizeInBytes(layoutElement.Format);
			layoutElement.SemanticName = sigParamDesc.SemanticName;
			layoutElement.SemanticIndex = sigParamDesc.SemanticIndex;
			alignedByteOffset += layoutElement.Size;
			m_Stride += layoutElement.Size;
			m_LayoutElements.push_back(layoutElement);
		}
	}

	std::vector<D3D12_INPUT_ELEMENT_DESC> VertexLayout::GetD3D12InputLayout() const
	{
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
		UINT inputSlot = 0;
		for(auto& element : m_LayoutElements)
		{
			D3D12_INPUT_ELEMENT_DESC desc;
			desc.Format = ShaderDataFormatHelper::ShaderDataFormatToDXGIFormat(element.Format);
			desc.AlignedByteOffset = element.AlignedByteOffset;
			desc.SemanticName = element.SemanticName.c_str();
			desc.SemanticIndex = element.SemanticIndex;
			desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			desc.InputSlot = 0;
			desc.InstanceDataStepRate = 0UL;
			inputLayout.push_back(desc);
		}

		return inputLayout;

	}

	void VertexLayout::PrintVertexLayoutInfo() const
	{
		for(auto &elem : m_LayoutElements)
		{
			acc3d_debug("SemanticName:{0}\nSemanticIndex:{1}\nRegister:{2}\nAlignedByteOffset:{3}\nFormat:{4}\nSize:{5}\n",
				elem.SemanticName,elem.SemanticIndex,elem.Register,elem.AlignedByteOffset,
				ShaderDataFormatHelper::ShaderDataFormatToString(elem.Format),elem.Size);
		}
	}
}
