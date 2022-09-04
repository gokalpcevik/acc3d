//
// Created by GOKALP on 9/2/2022.
//

#include "CommandAllocator.h"

namespace acc3d::Graphics
{
    CommandAllocator::CommandAllocator(ID3D12Device *pDevice, D3D12_COMMAND_LIST_TYPE type)
    {
        auto hr = pDevice->CreateCommandAllocator(type, IID_PPV_ARGS(&m_CmdAllocator));
        if (!SUCCEEDED(hr))
        {
            acc3d_error("Command Allocator  could not be created properly.");
            return;
        }

    }

    void CommandAllocator::Reset()
    {
        THROW_IFF(m_CmdAllocator->Reset());
    }

}