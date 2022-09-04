#include "Fence.h"

namespace acc3d
{
    namespace Graphics
    {
        Fence::Fence(ID3D12Device *pDevice, D3D12_FENCE_FLAGS flags,UINT64 initialValue)
        {
            THROW_IFF(pDevice->CreateFence(initialValue, flags, IID_PPV_ARGS(&m_Fence)));
        }

        void Fence::Reset()
        {
            THROW_IFF(m_Fence.Reset());
        }

        HANDLE Fence::CreateFenceEventHandle()
        {
            HANDLE fenceEvent = INVALID_HANDLE_VALUE;

            fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

            assert(fenceEvent);
            return fenceEvent;
        }

        ID3D12Fence *Fence::GetFence() const
        { return m_Fence.Get(); }


    } // acc3d
} // Graphics