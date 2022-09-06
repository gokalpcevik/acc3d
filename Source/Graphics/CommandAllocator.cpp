//
// Created by GOKALP on 9/2/2022.
//

#include "CommandAllocator.h"

namespace acc3d::Graphics
{
    CommandAllocator::CommandAllocator(ID3D12Device *pDevice, D3D12_COMMAND_LIST_TYPE type)
    {
        THROW_IFF(pDevice->CreateCommandAllocator(type, IID_PPV_ARGS(&m_CmdAllocator)));
    }

    ID3D12CommandAllocator* CommandAllocator::GetD3D12CommandAllocatorPtr() const
    { return m_CmdAllocator.Get(); }

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& CommandAllocator::GetD3D12CommandAllocator()
    {
        return m_CmdAllocator;
    }

    void CommandAllocator::Reset()
    {
        THROW_IFF(m_CmdAllocator->Reset());
    }

}