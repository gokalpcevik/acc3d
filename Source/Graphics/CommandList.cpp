//
// Created by GOKALP on 9/2/2022.
//

#include "CommandList.h"

namespace acc3d::Graphics
{
    GraphicsCommandList::GraphicsCommandList(ID3D12Device *pDevice,
                                             ID3D12CommandAllocator *pAllocator,
                                             D3D12_COMMAND_LIST_TYPE type,
                                             ID3D12PipelineState *pInitialState)
    {
        auto createResult = pDevice->CreateCommandList(0, type, pAllocator, pInitialState,
                                                       IID_PPV_ARGS(&m_CmdList));
        if (!SUCCEEDED(createResult))
        {
            acc3d_error("Graphics command list could not be created properly.");
            return;
        }

        // Command list is created in an open state and must be closed before it can be reset.
        if (SUCCEEDED(createResult))
        {
            D3D_CALL(m_CmdList->Close());
        }
    }

    void GraphicsCommandList::Reset(ID3D12CommandAllocator *pAllocator, ID3D12PipelineState *pPSO)
    {
        D3D_CALL(m_CmdList->Reset(pAllocator, pPSO));
    }

    ID3D12GraphicsCommandList *GraphicsCommandList::GetGraphicsCommandList() const
    { return m_CmdList.Get(); }
} // Graphics