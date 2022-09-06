#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include "../Core/Log.h"


namespace acc3d::Graphics
{
    class Device
    {
    public:
        Device();

        [[nodiscard]] ID3D12Device *GetD3D12DevicePtr();

        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Device2> GetD3D12Device2();

        IDXGIFactory3 *GetDXGIFactory();

        [[nodiscard]] bool IsTearingSupported() const;

    private:
        Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
        Microsoft::WRL::ComPtr<IDXGIFactory3> m_DXGIFactory;

        friend class RendererFactory;
    };
} // namespace acc3d
