#include "Synchronizer.h"

namespace acc3d::Graphics
{

	uint64_t Synchronizer::IncrementAndSignal(ID3D12CommandQueue* pCmdQueue, ID3D12Fence* pFence,
		uint64_t& fenceValue)
	{
		uint64_t fenceValueForSignal = ++fenceValue;
		D3D_CALL(pCmdQueue->Signal(pFence, fenceValueForSignal));
		return fenceValueForSignal;
	}

	void Synchronizer::WaitForFenceValue(ID3D12Fence* pFence, uint64_t fenceValue, HANDLE fenceEvent)
	{
		if(pFence->GetCompletedValue() < fenceValue)
		{
			D3D_CALL(pFence->SetEventOnCompletion(fenceValue, fenceEvent));
			::WaitForSingleObject(fenceEvent, static_cast<DWORD>(std::chrono::milliseconds::max().count()));
		}
	}

	void Synchronizer::Flush(ID3D12CommandQueue* pCmdQueue, ID3D12Fence* pFence, uint64_t& fenceValue, HANDLE fenceEvent)
	{
		uint64_t fenceValueForSignal = Synchronizer::IncrementAndSignal(pCmdQueue,pFence,fenceValue);
		Synchronizer::WaitForFenceValue(pFence, fenceValueForSignal, fenceEvent);
	}

}
