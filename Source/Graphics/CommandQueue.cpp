#include "CommandQueue.h"


namespace acc3d::Graphics
{
    CommandQueue::CommandQueue(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE commandListType)
	    : m_CmdListType(commandListType)
	{
        D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
        cmdQueueDesc.Type = commandListType;
        cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        cmdQueueDesc.NodeMask = 0;
        HRESULT hr = pDevice->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_CmdQueue));
        if (!SUCCEEDED(hr))
        {
            acc3d_error("Command queue could not be created properly.");
            return;
        }
    }

    ID3D12CommandQueue* CommandQueue::GetD3D12CommandQueuePtr() const
    {
        return m_CmdQueue.Get();
    }

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> const& CommandQueue::GetD3D12CommandQueue() const
    {
        return m_CmdQueue;
    }
} // namespace acc3d
