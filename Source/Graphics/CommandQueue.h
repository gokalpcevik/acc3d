#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include "CommandList.h"
#include "CommandAllocator.h"
#include "../Core/Log.h"

namespace acc3d::Graphics
{
    class CommandQueue
    {
    public:
        CommandQueue(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE commandListType);

        virtual ~CommandQueue() = default;

        [[nodiscard]] ID3D12CommandQueue* GetD3D12CommandQueuePtr() const;
        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12CommandQueue> const& GetD3D12CommandQueue() const;
    private:
        Microsoft::WRL::ComPtr<ID3D12CommandQueue>  m_CmdQueue;
        D3D12_COMMAND_LIST_TYPE m_CmdListType{};

        friend class RendererFactory;
    };
} // namespace acc3d
