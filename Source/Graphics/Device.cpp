#include "Device.h"

namespace acc3d::Graphics
{
    using Microsoft::WRL::ComPtr;

    Device::Device()
    {
        UINT8 flags = 0U;
#if defined(_DEBUG) || defined(DEBUG)
        ComPtr<ID3D12Debug3> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
        }
        flags = DXGI_CREATE_FACTORY_DEBUG;
#endif
        HRESULT hr = CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_DXGIFactory));
        if (FAILED(hr))
        {
            acc3d_error("Error while creating DXGI Factory!");
            return;
        }

        ComPtr<IDXGIFactory6> DXGIFactory6;
        hr = m_DXGIFactory->QueryInterface(__uuidof(IDXGIFactory6), &DXGIFactory6);
        if (FAILED(hr))
        {
            acc3d_error("acc3d requires at least Windows 10 April 2018 Update(17134) or later.");
            return;
        }

        bool dedicatedAdapterFound = false;
        ComPtr<IDXGIAdapter1> adapter;
        for (UINT adapterIndex = 0;
             DXGI_ERROR_NOT_FOUND != DXGIFactory6->EnumAdapterByGpuPreference(
                     adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)); ++
                     adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc{};
            hr = adapter->GetDesc1(&desc);
            if (FAILED(hr))
                continue;
            // Skip the basic driver adapter.
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                continue;

            if (SUCCEEDED(
                    D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
                                      _uuidof(ID3D12Device), nullptr)))
            {
                dedicatedAdapterFound = true;
                acc3d_trace("Using the adapter with the properties below:");
                constexpr size_t outputSize = _countof(desc.Description) + 1;
                auto adapterDescriptionPtr = new char[outputSize];
                size_t charsConverted = 0;
                const wchar_t *inputW = desc.Description;
                wcstombs_s(&charsConverted, adapterDescriptionPtr, outputSize, inputW, 128);
                acc3d_info("Description: {0}", adapterDescriptionPtr);
                delete[] adapterDescriptionPtr;
                acc3d_info("Dedicated Video Memory: {0}MB",
                           desc.DedicatedVideoMemory / 1024 / 1024);
                break;
            }
        }

        if (!dedicatedAdapterFound)
        {
            acc3d_error("Dedicated video adapter could not be found!");
            return;
        }

        hr = D3D12CreateDevice(adapter.Get(),
                               D3D_FEATURE_LEVEL_11_0,
                               IID_PPV_ARGS(&m_Device));

        if (FAILED(hr))
        {
            acc3d_error("Error while creating the D3D12Device!");
            return;
        }

        static const D3D_FEATURE_LEVEL s_featureLevels[] =
                {
                        D3D_FEATURE_LEVEL_12_1,
                        D3D_FEATURE_LEVEL_12_0,
                        D3D_FEATURE_LEVEL_11_1,
                        D3D_FEATURE_LEVEL_11_0,
                };

        D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels =
                {
                                _countof(s_featureLevels), s_featureLevels, D3D_FEATURE_LEVEL_11_0
                };

        hr = m_Device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS,
                                           &featLevels, sizeof(featLevels));
        if (SUCCEEDED(hr))
        {
            if (static_cast<uint64_t>(featLevels.MaxSupportedFeatureLevel) < static_cast<uint64_t>(
                    D3D_FEATURE_LEVEL_11_0))
            {
                acc3d_error("Specified feature level not supported!");
                return;
            }
        }
        acc3d_info("ID3D12Device created successfully.");
    }

    bool Device::IsTearingSupported() const
    {
        BOOL tearingAllowed = FALSE;
        ComPtr<IDXGIFactory5> factory5;
        if (SUCCEEDED(m_DXGIFactory.As(&factory5)))
        {
            if (FAILED(factory5->CheckFeatureSupport(
                    DXGI_FEATURE_PRESENT_ALLOW_TEARING,
                    &tearingAllowed, sizeof(tearingAllowed))))
            {
                tearingAllowed = FALSE;
            }
        }
        return tearingAllowed == TRUE;
    }

    ID3D12Device *Device::GetD3D12Device()
    { return m_Device.Get(); }

    IDXGIFactory3 *Device::GetDXGIFactory()
    { return m_DXGIFactory.Get(); }
}

// namespace acc3d
