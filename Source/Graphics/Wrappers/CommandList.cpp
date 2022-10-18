//
// Created by GOKALP on 9/2/2022.
//

#include "CommandList.h"

namespace acc3d::Graphics
{
    CommandList::CommandList(ID3D12Device *pDevice,
                                             ID3D12CommandAllocator *pAllocator,
                                             D3D12_COMMAND_LIST_TYPE type,
                                             ID3D12PipelineState *pInitialState)
	                                             : m_CmdListType(type)
	{
        THROW_IFF(pDevice->CreateCommandList(0, type, pAllocator, pInitialState,
            IID_PPV_ARGS(&m_CmdList)));
        THROW_IFF(m_CmdList->Close());
    }

    void CommandList::Reset(ID3D12CommandAllocator *pAllocator, ID3D12PipelineState *pPSO)
    {
        THROW_IFF(m_CmdList->Reset(pAllocator, pPSO));
    }

    void CommandList::Close()
    {
        THROW_IFF(m_CmdList->Close());
    }

    D3D12_COMMAND_LIST_TYPE CommandList::GetCommandListType() const
    {
        return m_CmdListType;
    }

    ID3D12GraphicsCommandList2 *CommandList::GetD3D12GraphicsCommandListPtr() const
    { return m_CmdList.Get(); }

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2>& CommandList::GetD3D12GraphicsCommandList()
    {
        return m_CmdList;
    }
} // Graphics