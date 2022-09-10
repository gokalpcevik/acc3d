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
#include <yaml-cpp/yaml.h>
#include "../RootSignature.h"
#include "../../Core/Log.h"

namespace acc3d::Graphics
{
    class RootSignatureParser
    {
    public:
        RootSignatureParser(ID3D12Device* pDevice, const std::filesystem::path& path, D3D12_ROOT_SIGNATURE_FLAGS flags);

        static std::optional<D3D12_ROOT_PARAMETER_TYPE> StringToRootParameterType(std::string_view str);
        static std::optional<D3D12_SHADER_VISIBILITY> StringToShaderVisibility(std::string_view str);
        static std::optional<D3D12_ROOT_DESCRIPTOR_FLAGS> StringToRootDescriptorFlag(std::string_view str);
        static std::optional<D3D12_DESCRIPTOR_RANGE_FLAGS> StringToDescriptorRangeFlag(std::string_view str);
        static std::optional<D3D12_DESCRIPTOR_RANGE_TYPE> StringToDescriptorRangeType(std::string_view str);

        static std::optional<CD3DX12_ROOT_PARAMETER1> GetD3D1232BitConstantsRootParameter(
            YAML::Node const& node, std::string_view parameterName, std::string_view visibilityStr,
            D3D12_SHADER_VISIBILITY visibility);

        static std::optional<CD3DX12_ROOT_PARAMETER1> GetD3D12CBV_SRV_UAV_RootParameter(
            YAML::Node const& node, std::string_view parameterName, std::string_view typeStr, std::string_view visibilityStr,
            D3D12_SHADER_VISIBILITY visibility);

        static std::optional<CD3DX12_ROOT_PARAMETER1> GetD3D12DescriptorTableRootParameter(
            YAML::Node const& node, std::string_view parameterName, std::string_view visibilityStr,
            D3D12_SHADER_VISIBILITY visibility);

        static std::optional<std::pair<int32_t, int32_t>> ValidateShaderRegisterAndRegisterSpace(YAML::Node const& node, std::string_view parameterName);
    private:
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
    };

}


namespace YAML
{
    template <typename T>
    struct as_if<T, std::optional<T> >
    {
        explicit as_if(const YAML::Node& node_) : node(node_) {}
        const YAML::Node& node;

        const std::optional<T> operator()() const
        {
            std::optional<T> val;
            T t;
            if (node.m_pNode && YAML::convert<T>::decode(node, t))
                val = std::move(t);

            return val;
        }
    };

    template <>
    struct as_if<std::string, std::optional<std::string> >
    {
        explicit as_if(const ::YAML::Node& node_) : node(node_) {}
        const ::YAML::Node& node;

        const std::optional<std::string> operator()() const
        {
            std::optional<std::string> val;
            std::string t;
            if (node.m_pNode && ::YAML::convert<std::string>::decode(node, t))
                val = std::move(t);

            return val;
        }
    };
}