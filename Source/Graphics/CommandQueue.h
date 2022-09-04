#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include "../Core/Log.h"

namespace acc3d::Graphics
{
    /*
     * Command Queue
     * */
    class CommandQueue
    {
    public:
        CommandQueue(ID3D12Device *pDevice, D3D12_COMMAND_LIST_TYPE commandType);

        virtual ~CommandQueue() = default;

        [[nodiscard]] ID3D12CommandQueue *GetCommandQueue() const;

    private:
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CmdQueue;
    };

} // namespace acc3d
