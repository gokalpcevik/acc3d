#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include "ResultHandler.h"
#include "RendererConfiguration.h"

namespace acc3d::Graphics
{
    struct DescriptorHeapSizeInfo
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

        [[nodiscard]] ID3D12DescriptorHeap *GetDescriptorHeap() const
        { return m_DescriptorHeap.Get(); }

        static std::pair<std::unique_ptr<DescriptorHeap>, std::unique_ptr<DescriptorHeap>>
        CreateDescriptorHeapsForSwapChainBuffers(ID3D12Device *pDevice);

        static DescriptorHeapSizeInfo GetDescriptorHeapSizeInfo(ID3D12Device *pDevice);


    private:
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
    };

} // Graphics

