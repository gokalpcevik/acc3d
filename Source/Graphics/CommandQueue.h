#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include <intsafe.h>
#include <queue>
#include "CommandList.h"
#include "CommandAllocator.h"
#include "../Core/Log.h"

namespace acc3d::Graphics
{
    /*
     * For every command allocator in flight, we need to track a fence value associated with it.
     * */
    class CommandQueue
    {
    public:
        CommandQueue(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE commandType);

        virtual ~CommandQueue() = default;

        [[nodiscard]] ID3D12CommandQueue* GetD3D12CommandQueue() const;
    private:
        Microsoft::WRL::ComPtr<ID3D12CommandQueue>  m_CmdQueue;

        friend class RendererFactory;
    };
} // namespace acc3d
