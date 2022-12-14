#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include "ResultHandler.h"
#include "../Core/Log.h"

namespace acc3d::Graphics
{
    /*
     * A command list object is used to issue draw, copy, compute commands. They are not executed
     * immediately but rather executed when they are executed on the command queue.
     * */

    class CommandList
    {
    public:
        // Type parameter must be of the same value as the CommandAllocator's command list type.
        CommandList(ID3D12Device *pDevice, ID3D12CommandAllocator *pAllocator,
                            D3D12_COMMAND_LIST_TYPE type, ID3D12PipelineState *pInitialState);

        CommandList(const CommandList &) = delete;

        CommandList(CommandList &&) = default;

        void Reset(ID3D12CommandAllocator *pAllocator, ID3D12PipelineState *pPSO);

        void Close();

        [[nodiscard]] D3D12_COMMAND_LIST_TYPE GetCommandListType() const;

        [[nodiscard]] ID3D12GraphicsCommandList2 *GetD3D12GraphicsCommandListPtr() const;
        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2>& GetD3D12GraphicsCommandList();

    private:
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> m_CmdList;
        D3D12_COMMAND_LIST_TYPE m_CmdListType{};
    };

} // Graphics
