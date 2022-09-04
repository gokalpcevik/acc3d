#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include "ResultHandler.h"
#include "../Core/Log.h"

namespace acc3d::Graphics
{
    /*
     * The Fence object is used to synchronize commands issued to the Command Queue.
     * The Fence object stores a single value that indicates the last value used to
     * signal the fence.
     * Although it is possible to use a single fence object to synchronize with multiple
     * command queues, it is not reliable to ensure the proper synchronization of commands
     * across different command queues.
     * In addition to the fence object, the application must also track a fence value that
     * is used to signal the fence.
     *
     * Internally, Fence object stores a single 64-bit unsigned integer value. The fence's initial
     * value(usually zero) is specified when the fence is created. The fence's internal value is updated
     * on the CPU using the ID3D12Fence::Signal() method and it is updated on the GPU using the
     * ID3D12CommandQueue::Signal() method.
     * To wait for a fence to reach a specific value on the CPU, use the ID3D12Fence::SetEventOnCompletion
     * method followed by a call to ::WaitForSingleObject() function.
     *
     * As a rule of thumb, the fence object's initial value should almost always be 0 and the fence value
     * should only be allowed to increase.
     * The fence is considered finished if it is equal or greater than a specific fence value.
     *
     * */
    class Fence
    {
    public:
        explicit Fence(ID3D12Device *pDevice, D3D12_FENCE_FLAGS flags, UINT64 initialValue);

        void Reset();

        static HANDLE CreateFenceEventHandle();

        [[nodiscard]] ID3D12Fence *GetFence() const;

    private:
        Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;

    };

} // Graphics
