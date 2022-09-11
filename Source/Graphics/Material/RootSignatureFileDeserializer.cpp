#include "RootSignatureFileDeserializer.h"


namespace acc3d::Graphics
{
	RootSignatureFileDeserializer::RootSignatureFileDeserializer(const std::filesystem::path& path)
	{
		acc3d_trace("Attempting to deserialize root signature file:\n		   {0}", path.string());

		YAML::Node root;

		try
		{
			root = YAML::LoadFile(path.string());
		}
		catch (YAML::ParserException const& e)
		{
			acc3d_error("Error deserializing root signature description file: {0}", path.string(), e.what());
			return;
		}

		if (!root["root_signature_description"])
		{
			acc3d_error("No 'root_signature_description' node found in the root signature file root. File: {0}",
			            path.string());
			return;
		}

		YAML::Node const desc = root["root_signature_description"];

		if (!desc)
		{
			acc3d_error("No 'root_signature_description' node has been found.");
			return;
		}

		std::string rootSigName{};

		if (!desc["root_signature_name"])
		{
			acc3d_warn("'root_signature_name' has not been set. Defaulting to 'Unnamed Root Signature'");
			rootSigName = "Unnamed Root Signature";
		}

		if (!desc["root_parameters"])
		{
			acc3d_error("No 'root_parameters' node has been found in the root signature file. File:\n{0}",
			            path.string());
			return;
		}

		YAML::Node const rootParameters = desc["root_parameters"];

		if (!rootParameters.IsSequence())
		{
			acc3d_error("'root_parameters' needs to be sequence.");
			return;
		}

		size_t i = 0;

		for (auto param : rootParameters)
		{
			YAML::Node paramNameNode = param["parameter_name"];
			if (!paramNameNode)
			{
				acc3d_error("Parameter name needs to be set for the root parameter at index {0}.", i);
				return;
			}

			auto paramNameStr = yaml::AsIf<std::string>(paramNameNode);
			if (!paramNameStr)
			{
				acc3d_error("Parameter name is not a string.");
				return;
			}

			YAML::Node typeNode = param["type"];
			if (!typeNode)
			{
				acc3d_error("No root parameter 'type' has been set for root parameter '{0}'.", paramNameStr.value());
				return;
			}
			YAML::Node visibilityNode = param["visibility"];
			if (!visibilityNode)
			{
				acc3d_error("No root parameter 'visibility' has been set for root parameter '{0}'.",
				            paramNameStr.value());
				return;
			}

			auto typeStr = yaml::AsIf<std::string>(typeNode);
			auto paramType = RootSignatureFileDeserializer::StringToRootParameterType(*typeStr);
			

			/* Check the 'type' string values to see if they are valid. */
			{
				if (!typeStr)
				{
					acc3d_error("Current type is not a string for the parameter '{0}'", paramNameStr.value());
					return;
				}
				if (!paramType)
				{
					acc3d_error("Type '{1}' is not a valid type for the root parameter '{0}'", paramNameStr.value(),
					            *typeStr);
					return;
				}
			}

			auto visibilityStr = yaml::AsIf<std::string>(visibilityNode);
			auto visibility = RootSignatureFileDeserializer::StringToShaderVisibility(*visibilityStr);

			/* Check the 'visibility' string values to see if they are valid. */
			{
				if (!visibilityStr)
				{
					acc3d_error("Current visibility setting is not a string for the parameter '{0}'",
					            paramNameStr.value());
					return;
				}

				if (!visibility)
				{
					acc3d_error("Visibility setting '{1}' is not a valid value for the parameter '{0}'.",
					            paramNameStr.value(), *visibilityStr);
					return;
				}
			}
			
			std::optional<CD3DX12_ROOT_PARAMETER1> d3d12RootParameter;

			switch (*paramType)
			{
			case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
				{
					m_DescriptorRanges.emplace_back();
					d3d12RootParameter = GetD3D12DescriptorTableRootParameter(
						param,i ,*paramNameStr, *visibilityStr, *visibility);
					if (!d3d12RootParameter)
						return;
					break;
				}
			case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
				{
					d3d12RootParameter = GetD3D1232BitConstantsRootParameter(
						param, *paramNameStr, *visibilityStr, *visibility);
					if (!d3d12RootParameter)
						return;
					break;
				}
			case D3D12_ROOT_PARAMETER_TYPE_CBV:
			case D3D12_ROOT_PARAMETER_TYPE_SRV:
			case D3D12_ROOT_PARAMETER_TYPE_UAV:
				{
					d3d12RootParameter = GetD3D12CBV_SRV_UAV_RootParameter(
						param, *paramNameStr, *typeStr, *visibilityStr, *visibility);
					if (!d3d12RootParameter)
						return;
					break;
				}

			default: break;
			}

			m_RootParameters.push_back(*d3d12RootParameter);


			++i;
		}

		m_ParseSucceeded = true;
	}

	std::optional<CD3DX12_ROOT_PARAMETER1> RootSignatureFileDeserializer::GetD3D1232BitConstantsRootParameter(
		YAML::Node const& node, std::string_view parameterName, std::string_view visibilityStr,
		D3D12_SHADER_VISIBILITY visibility)
	{
		auto shaderRegister_RegisterSpacePair = ValidateShaderRegisterAndRegisterSpace(node, parameterName);
		if (!shaderRegister_RegisterSpacePair)
			return std::nullopt;
		auto [shaderRegister, registerSpace] = *shaderRegister_RegisterSpacePair;

		YAML::Node const num32bitValNode = node["num_32_bit_val"];
		std::optional<int32_t> const num32BitVal = yaml::AsIf<int32_t>(num32bitValNode);

		if (!(num32bitValNode && num32BitVal))
		{
			acc3d_error(
				R"(Ill-formed root parameter named '{0}'. Check if you have defined or correctly defined 'num_32_bit_val')",
				parameterName);
			return std::nullopt;
		}

		acc3d_debug(
			"root_parameter_name={}\ntype=32bit_constants\nvisibility={}\nnum_32_bit_val={}\nshader_register={}\nregister_space={}\n",
			parameterName, visibilityStr, *num32BitVal, shaderRegister,
			registerSpace);
		CD3DX12_ROOT_PARAMETER1 param1{};
		param1.InitAsConstants(*num32BitVal, shaderRegister, registerSpace, visibility);
		return param1;
	}

	std::optional<CD3DX12_ROOT_PARAMETER1> RootSignatureFileDeserializer::GetD3D12CBV_SRV_UAV_RootParameter(
		YAML::Node const& node, std::string_view parameterName, std::string_view typeStr,
		std::string_view visibilityStr,
		D3D12_SHADER_VISIBILITY visibility)
	{
		auto shaderRegister_RegisterSpacePair = ValidateShaderRegisterAndRegisterSpace(node, parameterName);
		if (!shaderRegister_RegisterSpacePair)
			return std::nullopt;
		auto [shaderRegister, registerSpace] = *shaderRegister_RegisterSpacePair;

		YAML::Node const rootDescriptorFlagNode = node["root_descriptor_flags"];
		std::optional<std::string> const rootDescriptorFlagStr = yaml::AsIf<std::string>(rootDescriptorFlagNode);

		if (!rootDescriptorFlagNode)
		{
			acc3d_error(R"(Ill-formed root parameter named '{0}'. Check if you have defined 'root_descriptor_flags'.)",
			            parameterName);
			return std::nullopt;
		}

		if (!rootDescriptorFlagStr)
		{
			acc3d_error("Root descriptor flag is not a string in root parameter '{0}.'", parameterName);
			return std::nullopt;
		}

		auto const rootDescriptorFlag = StringToRootDescriptorFlag(*rootDescriptorFlagStr);

		if (!(rootDescriptorFlagNode && rootDescriptorFlag))
		{
			acc3d_error(
				R"(Ill-formed root parameter named '{0}'. Check if you have defined or correctly defined 'root_descriptor_flag.')",
				parameterName);
			return std::nullopt;
		}
		acc3d_debug(
			"root_parameter_name={}\ntype={}\nvisibility={}\nroot_descriptor_flags={}\nshader_register={}\nregister_space={}\n",
			parameterName, typeStr, visibilityStr, *rootDescriptorFlagStr, shaderRegister,
			registerSpace);
		CD3DX12_ROOT_PARAMETER1 param1{};

		// SWITCH
		param1.InitAsConstantBufferView(shaderRegister, registerSpace, *rootDescriptorFlag, visibility);
		return param1;
	}

	std::optional<CD3DX12_ROOT_PARAMETER1> RootSignatureFileDeserializer::GetD3D12DescriptorTableRootParameter(
		YAML::Node const& node, size_t index, std::string_view parameterName, std::string_view visibilityStr,
		D3D12_SHADER_VISIBILITY visibility)
	{
		YAML::Node const descriptorRangesNode = node["descriptor_ranges"];

		if (!(descriptorRangesNode && descriptorRangesNode.IsSequence()))
		{
			acc3d_error(
				"Ill-formed root parameter named '{0}', Check if you have defined a 'ranges' sequence or it is a sequence.",
				parameterName);
			return std::nullopt;
		}

		size_t i = 0;

		for (auto rangeNode : descriptorRangesNode)
		{
			if (!rangeNode.IsMap())
			{
				acc3d_error("Descriptor range node at index '{0}' in the root parameter '{1}' is not a map.", i,
				            parameterName);
				return std::nullopt;
			}
			YAML::Node rangeNameNode = rangeNode["range_name"];
			YAML::Node numDescriptorsNode = rangeNode["num_descriptors"];
			YAML::Node baseShaderRegisterNode = rangeNode["base_shader_register"];
			YAML::Node registerSpaceNode = rangeNode["register_space"];
			YAML::Node descriptorRangeTypeNode = rangeNode["descriptor_range_type"];
			YAML::Node descriptorRangeFlagsNode = rangeNode["descriptor_range_flags"];

			if (!(rangeNameNode && numDescriptorsNode && baseShaderRegisterNode &&
				registerSpaceNode && descriptorRangeTypeNode && descriptorRangeFlagsNode))
			{
				acc3d_error(
					R"(Ill-formed descriptor range at index '{0}' in the root parameter '{1}'. Check if you have defined 'range_name', 'num_descriptors', 'base_shader_register', 'register_space','descriptor_range_type','descriptor_range_flags'.)",
					i, parameterName);
				return std::nullopt;
			}

			auto rangeName = yaml::AsIf<std::string>(rangeNameNode);
			auto numDescriptors = yaml::AsIf<int32_t>(numDescriptorsNode);
			auto baseShaderRegister = yaml::AsIf<int32_t>(baseShaderRegisterNode);
			auto registerSpace = yaml::AsIf<int32_t>(registerSpaceNode);
			auto descriptorRangeTypeStr = yaml::AsIf<std::string>(descriptorRangeTypeNode);
			auto descriptorRangeFlagsStr = yaml::AsIf<std::string>(descriptorRangeFlagsNode);

			if (!(rangeName && numDescriptors && baseShaderRegister &&
				registerSpace && descriptorRangeTypeStr && descriptorRangeFlagsStr))
			{
				acc3d_error(
					R"(Ill-formed descriptor range at index '{0}' in the root parameter '{1}'. Check if you have correctly defined,respecting the types, 'range_name', 'num_descriptors', 'base_shader_register', 'register_space','descriptor_range_type','descriptor_range_flags'.)",
					i, parameterName);
				return std::nullopt;
			}

			auto descriptorRangeType = StringToDescriptorRangeType(*descriptorRangeTypeStr);
			auto descriptorRangeFlags = StringToDescriptorRangeFlag(*descriptorRangeFlagsStr);

			if (!(descriptorRangeType && descriptorRangeFlags))
			{
				acc3d_error(
					"The values of 'descriptor_range_type' or 'descriptor_range_flags' are not correctly set in the range name '{0}'in the root parameter '{1}'.",
					*rangeName, parameterName);
				return std::nullopt;
			}

			CD3DX12_DESCRIPTOR_RANGE1 range{};
			range.Init(*descriptorRangeType, (UINT)*numDescriptors, (UINT)*baseShaderRegister, (UINT)*registerSpace,
			           *descriptorRangeFlags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);


			acc3d_debug(
				"root_parameter_name={7}\nrange_name={0}\nindex={1}\nnum_descriptors={2}\nbase_shader_register={3}\nregister_space={4}\ndescriptor_range_type={5}\ndescriptor_range_flags={6}\n"
				, *rangeName, i, *numDescriptors, *baseShaderRegister, *registerSpace, *descriptorRangeTypeStr,
				*descriptorRangeFlagsStr, parameterName);


			m_DescriptorRanges[index].push_back(range);
			i++;
		}

		CD3DX12_ROOT_PARAMETER1 param1{};
		param1.InitAsDescriptorTable((UINT)descriptorRangesNode.size(), m_DescriptorRanges[index].data(), visibility);
		return param1;
	}


	std::optional<std::pair<int32_t, int32_t>> RootSignatureFileDeserializer::ValidateShaderRegisterAndRegisterSpace(
		YAML::Node const& node, std::string_view parameterName)
	{
		YAML::Node const shaderRegisterNode = node["shader_register"];
		std::optional<int32_t> const shaderRegister = yaml::AsIf<int32_t>(shaderRegisterNode);
		YAML::Node const registerSpaceNode = node["register_space"];
		std::optional<int32_t> const registerSpace = yaml::AsIf<int32_t>(registerSpaceNode);

		if (!(shaderRegisterNode && registerSpaceNode && shaderRegister && registerSpace))
		{
			acc3d_error(
				R"(Ill-formed root parameter named '{0}'. Check if you have defined or correctly defined 'shader_register', 'register_space')",
				parameterName);
			return std::nullopt;
		}
		return {{*shaderRegister, *registerSpace}};
	}

	std::vector<CD3DX12_ROOT_PARAMETER1> const& RootSignatureFileDeserializer::GetDeserializedRootParameters() const
	{
		return m_RootParameters;
	}

	std::optional<D3D12_ROOT_PARAMETER_TYPE> RootSignatureFileDeserializer::StringToRootParameterType(
		std::string_view str)
	{
		if (str == "32bit_constants")
			return D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		else if (str == "descriptor_table")
			return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		else if (str == "constant_buffer_view")
			return D3D12_ROOT_PARAMETER_TYPE_CBV;
		else if (str == "shader_resource_view")
			return D3D12_ROOT_PARAMETER_TYPE_SRV;
		else if (str == "unordered_access_view")
			return D3D12_ROOT_PARAMETER_TYPE_UAV;
		else
			return std::nullopt;
	}

	std::optional<D3D12_SHADER_VISIBILITY> RootSignatureFileDeserializer::StringToShaderVisibility(std::string_view str)
	{
		if (str == "vertex")
			return D3D12_SHADER_VISIBILITY_VERTEX;
		else if (str == "pixel")
			return D3D12_SHADER_VISIBILITY_PIXEL;
		else if (str == "hull")
			return D3D12_SHADER_VISIBILITY_HULL;
		else if (str == "mesh")
			return D3D12_SHADER_VISIBILITY_MESH;
		else if (str == "domain")
			return D3D12_SHADER_VISIBILITY_DOMAIN;
		else if (str == "amplification")
			return D3D12_SHADER_VISIBILITY_AMPLIFICATION;
		else if (str == "geometry")
			return D3D12_SHADER_VISIBILITY_GEOMETRY;
		else if (str == "all")
			return D3D12_SHADER_VISIBILITY_ALL;
		else
			return std::nullopt;
	}

	std::optional<D3D12_ROOT_DESCRIPTOR_FLAGS> RootSignatureFileDeserializer::StringToRootDescriptorFlag(
		std::string_view str)
	{
		if (str == "none")
			return D3D12_ROOT_DESCRIPTOR_FLAG_NONE;
		else if (str == "data_static")
			return D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC;
		else if (str == "data_static_while_set_at_execute")
			return D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
		else if (str == "data_volatile")
			return D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE;
		else
			return std::nullopt;
	}

	std::optional<D3D12_DESCRIPTOR_RANGE_FLAGS> RootSignatureFileDeserializer::StringToDescriptorRangeFlag(
		std::string_view str)
	{
		if (str == "none")
			return D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
		else if (str == "data_static")
			return D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
		else if (str == "data_static_while_set_at_execute")
			return D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
		else if (str == "data_volatile")
			return D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE;
		else if (str == "descriptors_static_keeping_buffer_bounds_checks")
			return D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_STATIC_KEEPING_BUFFER_BOUNDS_CHECKS;
		else if (str == "descriptors_volatile")
			return D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
		else
			return std::nullopt;
	}

	std::optional<D3D12_DESCRIPTOR_RANGE_TYPE> RootSignatureFileDeserializer::StringToDescriptorRangeType(
		std::string_view str)
	{
		if (str == "descriptor_range_cbv")
			return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		else if (str == "descriptor_range_srv")
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		else if (str == "descriptor_range_uav")
			return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		else if (str == "descriptor_range_sampler")
			return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		else
			return std::nullopt;
	}
}
