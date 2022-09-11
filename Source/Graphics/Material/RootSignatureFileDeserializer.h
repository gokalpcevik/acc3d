#pragma once
#include <fstream>
#include <sstream>
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <filesystem>
#include <cstdint>
#include <optional>
#include <algorithm>
#include "acc3d_yaml.h"
#include "../../Core/Log.h"

namespace acc3d::Graphics
{
    class RootSignatureFileDeserializer
    {
    public:
        RootSignatureFileDeserializer(const std::filesystem::path& path);

        explicit operator bool() const { return m_ParseSucceeded; }


        [[nodiscard]] std::vector<CD3DX12_ROOT_PARAMETER1> const& GetDeserializedRootParameters() const;
        [[nodiscard]] D3D12_ROOT_SIGNATURE_FLAGS GetDeserializedRootSignatureFlags() const;

        static std::optional<D3D12_ROOT_PARAMETER_TYPE> StringToRootParameterType(std::string_view str);
        static std::optional<D3D12_SHADER_VISIBILITY> StringToShaderVisibility(std::string_view str);
        static std::optional<D3D12_ROOT_DESCRIPTOR_FLAGS> StringToRootDescriptorFlag(std::string_view str);
        static std::optional<D3D12_DESCRIPTOR_RANGE_FLAGS> StringToDescriptorRangeFlag(std::string_view str);
        static std::optional<D3D12_DESCRIPTOR_RANGE_TYPE> StringToDescriptorRangeType(std::string_view str);
        static std::optional<D3D12_ROOT_SIGNATURE_FLAGS> StringToRootSignatureFlags(std::string_view str);

        static std::optional <D3D12_ROOT_SIGNATURE_FLAGS> GetRootSignatureFlags(YAML::Node const& node);

        static std::optional<CD3DX12_ROOT_PARAMETER1> GetD3D1232BitConstantsRootParameter(
            YAML::Node const& node, std::string_view parameterName, std::string_view visibilityStr,
            D3D12_SHADER_VISIBILITY visibility);

        static std::optional<CD3DX12_ROOT_PARAMETER1> GetD3D12CBV_SRV_UAV_RootParameter(
            YAML::Node const& node, std::string_view parameterName, std::string_view typeStr, std::string_view visibilityStr,
            D3D12_SHADER_VISIBILITY visibility);

        std::optional<CD3DX12_ROOT_PARAMETER1> GetD3D12DescriptorTableRootParameter(
            YAML::Node const& node,size_t index ,std::string_view parameterName, std::string_view visibilityStr,
            D3D12_SHADER_VISIBILITY visibility);

        static std::optional<std::pair<int32_t, int32_t>> ValidateShaderRegisterAndRegisterSpace(YAML::Node const& node, std::string_view parameterName);

    private:
        D3D12_ROOT_SIGNATURE_FLAGS m_RootSignatureFlags;
        std::vector<CD3DX12_ROOT_PARAMETER1> m_RootParameters{};
        std::vector<std::vector<CD3DX12_DESCRIPTOR_RANGE1>> m_DescriptorRanges;
    	bool m_ParseSucceeded = false;
    };

}