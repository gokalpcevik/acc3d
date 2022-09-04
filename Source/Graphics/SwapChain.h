#pragma once

#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl/client.h>
#include "ResultHandler.h"
#include "Device.h"
#include "../Core/Window.h"
#include "RendererConfiguration.h"

namespace acc3d::Graphics
{

    class SwapChain
    {
    public:
        SwapChain(IDXGIFactory3 *pDXGIFactory, ID3D12CommandQueue *pCmdQueue,
                  Core::Window const &window, Device *pDevice);

        [[nodiscard]] IDXGISwapChain1 *GetDXGISwapChain() const;

        [[nodiscard]] UINT GetCurrentBackBufferIndex() const;

        void Present(UINT syncInterval, UINT presentFlags);

    private:
        Microsoft::WRL::ComPtr<IDXGISwapChain1> m_DXGISwapChain;
    };

} // Graphics

