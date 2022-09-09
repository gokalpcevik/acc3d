#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include "ResultHandler.h"
#include "RendererConfiguration.h"

namespace acc3d::Graphics
{
    struct DescriptorSizeInfo
    {
        UINT RTVDescriptorSize = 0UL;
        UINT DSVDescriptorSize = 0UL;
        UINT CBV_SRV_UAVDescriptorSize = 0UL;
        UINT SamplerDescriptorSize = 0UL;
    };

    class DescriptorHeap
    {
    public:
        DescriptorHeap(ID3D12Device *pDevice, const D3D12_DESCRIPTOR_HEAP_DESC &desc);
        DescriptorHeap(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, D3D12_DESCRIPTOR_HEAP_TYPE type);

        [[nodiscard]] ID3D12DescriptorHeap* GetD3D12DescriptorHeapPtr() const;
        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& GetD3D12DescriptorHeap();

        [[nodiscard]] D3D12_DESCRIPTOR_HEAP_TYPE GetDescriptorHeapType() const noexcept;
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart() const noexcept;

        static std::pair<std::unique_ptr<DescriptorHeap>, std::unique_ptr<DescriptorHeap>>
        CreateDescriptorHeapsForSwapChainBuffers(ID3D12Device *pDevice);

        static DescriptorSizeInfo GetDescriptorSizeInfo(ID3D12Device *pDevice) noexcept;


    private:
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
        D3D12_DESCRIPTOR_HEAP_TYPE m_DescriptorHeapType{};
    };

} // Graphics

