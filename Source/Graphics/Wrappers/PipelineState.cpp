#include "PipelineState.h"

namespace acc3d::Graphics
{
	PipelineState::PipelineState(ID3D12Device2* pDevice, D3D12_PIPELINE_STATE_STREAM_DESC const* pDesc)
	{
		THROW_IFF(pDevice->CreatePipelineState(pDesc, IID_PPV_ARGS(&m_PipelineState)));
	}

	Microsoft::WRL::ComPtr<ID3D12PipelineState> const& PipelineState::GetD3D12PipelineState() const
	{
		return m_PipelineState;
	}
}
