#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include "RendererConfiguration.h"
#include "ResultHandler.h"
#include "../Core/Log.h"
#include "../Core/Window.h"
#include "../Util/StringUtil.h"


namespace acc3d::Graphics
{
    class Device
    {
    public:
        Device();

        ID3D12Device *GetD3D12Device();

        IDXGIFactory3 *GetDXGIFactory();

        [[nodiscard]] bool IsTearingSupported() const;

    private:

    private:
        Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
        Microsoft::WRL::ComPtr<IDXGIFactory3> m_DXGIFactory;

        friend class RendererFactory;
        friend class Renderer;
    };
} // namespace acc3d
