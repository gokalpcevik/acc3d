#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include "../ResultHandler.h"
#include "../../Core/Log.h"

namespace acc3d::Graphics
{
    class CommandAllocator
    {
    public:
        CommandAllocator(ID3D12Device *pDevice, D3D12_COMMAND_LIST_TYPE type);

        [[nodiscard]] ID3D12CommandAllocator *GetD3D12CommandAllocatorPtr() const;
        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& GetD3D12CommandAllocator();
        void Reset();

    private:
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CmdAllocator;
    };

} // Graphics
