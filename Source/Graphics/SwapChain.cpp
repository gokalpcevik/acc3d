//
// Created by GOKALP on 9/3/2022.
//

#include "SwapChain.h"

namespace acc3d::Graphics
{
    SwapChain::SwapChain(IDXGIFactory3 *pDXGIFactory, ID3D12CommandQueue *pCmdQueue,
                         Core::Window const &window, Device *pDevice)
    {
        auto hwnd = window.GetWin32WindowHandle();

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = window.GetWidth();
        swapChainDesc.Height = window.GetHeight();
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = g_NUM_FRAMES_IN_FLIGHT;
        swapChainDesc.Scaling = DXGI_SCALING_NONE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDesc.Flags = pDevice->IsTearingSupported() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
                                                            : 0;

        HRESULT hr = pDXGIFactory->CreateSwapChainForHwnd(pCmdQueue, hwnd,
                                                          &swapChainDesc, nullptr, nullptr,
                                                          m_DXGISwapChain.ReleaseAndGetAddressOf());

        hr = pDXGIFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
        if (FAILED(hr))
        {
            acc3d_warn("IDXGIFactory::MakeWindowAssociation failed in file {0}:{1}", __FILE__,
                       __LINE__);
        }
        if (SUCCEEDED(hr))
        {
            acc3d_info("IDXGISwapChain creation successful.");
            return;
        }
    }

    UINT SwapChain::GetCurrentBackBufferIndex() const
    {
        Microsoft::WRL::ComPtr<IDXGISwapChain3> pSwapChain3;
        THROW_IFF(m_DXGISwapChain.As(&pSwapChain3));
        return pSwapChain3->GetCurrentBackBufferIndex();
    }

    void SwapChain::Present(UINT syncInterval, UINT presentFlags) const
    {
        THROW_IFF(m_DXGISwapChain->Present(syncInterval, presentFlags));
    }

    IDXGISwapChain1 *SwapChain::GetDXGISwapChain() const
    { return m_DXGISwapChain.Get(); }
} // Graphics