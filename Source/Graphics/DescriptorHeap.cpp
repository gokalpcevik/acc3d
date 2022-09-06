#include "DescriptorHeap.h"

namespace acc3d::Graphics
{
    DescriptorHeap::DescriptorHeap(ID3D12Device *pDevice,
                                   const D3D12_DESCRIPTOR_HEAP_DESC &desc)
	                                   : m_DescriptorHeapType(desc.Type)
	{
        THROW_IFF(pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_DescriptorHeap)));
    }

    ID3D12DescriptorHeap* DescriptorHeap::GetD3D12DescriptorHeapPtr() const
    { return m_DescriptorHeap.Get(); }

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& DescriptorHeap::GetD3D12DescriptorHeap()
    {
        return m_DescriptorHeap;
    }

    D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeap::GetDescriptorHeapType() const noexcept
    {
        return m_DescriptorHeapType;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUDescriptorHandleForHeapStart() const noexcept
    {
        return m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
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

    DescriptorSizeInfo DescriptorHeap::GetDescriptorSizeInfo(ID3D12Device *pDevice) noexcept
    {
        DescriptorSizeInfo info{};
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