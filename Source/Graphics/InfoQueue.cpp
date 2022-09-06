#include "InfoQueue.h"

namespace acc3d::Graphics
{
	InfoQueue::InfoQueue(ID3D12Device* pDevice)
	{
		THROW_IFF(pDevice->QueryInterface(IID_PPV_ARGS(&m_InfoQueue)));

		auto const& pInfoQueue = m_InfoQueue;
		pInfoQueue->ClearStoredMessages();
		pInfoQueue->ClearRetrievalFilter();
		pInfoQueue->ClearStorageFilter();
		pInfoQueue->PushEmptyRetrievalFilter();
		pInfoQueue->PushEmptyStorageFilter();

		typedef HRESULT (WINAPI* LPDXGIGETDEBUGINTERFACE)(REFIID, void**);

		HMODULE dxgidebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
		auto const dxgiGetDebugInterface = reinterpret_cast<LPDXGIGETDEBUGINTERFACE>(
			reinterpret_cast<void*>(GetProcAddress(dxgidebug, "DXGIGetDebugInterface")));

		dxgiGetDebugInterface(IID_PPV_ARGS(&m_DXGIInfoQueue));

		assert(dxgidebug && dxgiGetDebugInterface && m_DXGIInfoQueue);

		m_DXGIInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
		m_DXGIInfoQueue->ClearStoredMessages(DXGI_DEBUG_ALL);
		m_DXGIInfoQueue->ClearRetrievalFilter(DXGI_DEBUG_ALL);
		m_DXGIInfoQueue->ClearStorageFilter(DXGI_DEBUG_ALL);
		m_DXGIInfoQueue->PushEmptyStorageFilter(DXGI_DEBUG_ALL);
		m_DXGIInfoQueue->PushEmptyRetrievalFilter(DXGI_DEBUG_ALL);
	}

	void InfoQueue::FlushQueue() const
	{
		auto const numStoredMessages = m_InfoQueue->GetNumStoredMessages();

		for (int i = 0; i < numStoredMessages; ++i)
		{
			SIZE_T msgLen;
			if (SUCCEEDED(m_InfoQueue->GetMessage(i, nullptr, &msgLen)))
			{
				auto* msg = static_cast<D3D12_MESSAGE*>(malloc(msgLen));
				m_InfoQueue->GetMessage(i, msg, &msgLen);

				switch (msg->Severity)
				{
				case D3D12_MESSAGE_SEVERITY_CORRUPTION:
					{
						acc3d_error("{0} \n", msg->pDescription);
						break;
					}
				case D3D12_MESSAGE_SEVERITY_ERROR:
					{
						acc3d_error("{0} \n", msg->pDescription);
						break;
					}
				case D3D12_MESSAGE_SEVERITY_WARNING:
					{
						acc3d_warn("{0} \n", msg->pDescription);
						break;
					}
				case D3D12_MESSAGE_SEVERITY_INFO:
					break;
				case D3D12_MESSAGE_SEVERITY_MESSAGE:
					{
						acc3d_trace("{0} \n", msg->pDescription);
						break;
					}
				default:
					break;
				}
				free(msg);
			}
		}
		m_InfoQueue->ClearStoredMessages();


		const auto numStoredMessagesDXGI = m_DXGIInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

		for (int i = 0; i < numStoredMessagesDXGI; ++i)
		{
			SIZE_T msgLen;
			if (SUCCEEDED(m_DXGIInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &msgLen)))
			{
				auto* msg = static_cast<DXGI_INFO_QUEUE_MESSAGE*>(malloc(msgLen));
				m_DXGIInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, msg, &msgLen);

				switch (msg->Severity)
				{
				case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION:
					{
						acc3d_error("{0} \n", msg->pDescription);
						break;
					}
				case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR:
					{
						acc3d_error("{0} \n", msg->pDescription);
						break;
					}
				case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING:
					{
						acc3d_warn("{0} \n", msg->pDescription);
						break;
					}
				case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO:
					break;
				case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE:
					break;
				}
				free(msg);
			}
		}
		m_DXGIInfoQueue->ClearStoredMessages(DXGI_DEBUG_ALL);
	}
}
