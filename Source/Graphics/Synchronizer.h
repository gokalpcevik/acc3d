#pragma once
#define NOMINMAX

#include <d3d12.h>
#include <cassert>
#include <cstdint>
#include "ResultHandler.h"

#undef NOMINMAX

namespace acc3d::Graphics
{
    class Synchronizer
    {
    public:
        Synchronizer() = default;

        static uint64_t IncrementAndSignal(ID3D12CommandQueue *pCmdQueue, ID3D12Fence *pFence,
                                           uint64_t &fenceValue);

        static void WaitForFenceValue(ID3D12Fence *pFence, uint64_t fenceValue, HANDLE fenceEvent);

        static void Flush(ID3D12CommandQueue *pCmdQueue, ID3D12Fence *pFence, uint64_t &fenceValue,
                          HANDLE fenceEvent);
    };


}