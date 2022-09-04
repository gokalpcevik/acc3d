#include "DescriptorHeap.h"

namespace acc3d::Graphics
{
    DescriptorHeap::DescriptorHeap(ID3D12Device *pDevice,
                                   const D3D12_DESCRIPTOR_HEAP_DESC &desc)
    {
        D3D_CALL(pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_DescriptorHeap)));
    }

    std::pair<std::unique_ptr<DescriptorHeap>, std::unique_ptr<DescriptorHeap>>
    DescriptorHeap::CreateDescriptorHeapsForSwapChainBuffers(ID3D12Device *pDevice)
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvDesc{};
        D3D12_DESCRIPTOR_HEAP_DESC dsvDesc{};

        rtvDesc.NumDescriptors = g_NUM_FRAMES_IN_FLIGHT;
        rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtvDesc.NodeMask = 0;

        dsvDesc.NumDescriptors = 1;
        dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        dsvDesc.NodeMask = 0;

        auto rtvDescriptorHeap = std::make_unique<DescriptorHeap>(
                pDevice, rtvDesc);
        auto dsvDescriptorHeap = std::make_unique<DescriptorHeap>(
                pDevice, dsvDesc);

        return {std::move(rtvDescriptorHeap), std::move(dsvDescriptorHeap)};
    }

    DescriptorHeapSizeInfo DescriptorHeap::GetDescriptorHeapSizeInfo(ID3D12Device *pDevice)
    {
        DescriptorHeapSizeInfo info{};
        info.RTVDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(
                D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        info.DSVDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(
                D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        info.CBV_SRV_UAVDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(
                D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        info.SamplerDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(
                D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
        return info;
    }

} // Graphics