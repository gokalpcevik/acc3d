#pragma once
#include <d3d12.h>
#include <dxgidebug.h>
#include <wrl.h>
#include "../ResultHandler.h"
#include "../../Core/Log.h"

namespace acc3d::Graphics
{
	class InfoQueue
	{
	public:
		InfoQueue(ID3D12Device* pDevice);


		void FlushQueue() const;
	private:
		Microsoft::WRL::ComPtr<ID3D12InfoQueue> m_InfoQueue;
		Microsoft::WRL::ComPtr<IDXGIInfoQueue> m_DXGIInfoQueue;
	};
}
