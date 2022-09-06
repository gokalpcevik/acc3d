#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include "ResultHandler.h"

namespace acc3d::Graphics
{
	class PipelineState
	{
	public:
		PipelineState(ID3D12Device2* pDevice, D3D12_PIPELINE_STATE_STREAM_DESC const* pDesc);


	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;

	};
}