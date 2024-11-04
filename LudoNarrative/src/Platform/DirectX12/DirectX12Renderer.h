#pragma once

#include "Ludo/Renderer/InternalRenderer.h"

#include "Ludo/Log.h"

namespace Ludo {

	class DirectX12Renderer : public InternalRenderer
	{
	public:
		bool Init() override;
		~DirectX12Renderer() override;

		void BeginImGui() override;
		void EndImGui() override;

		auto* const& InitCommandList()
		{
			m_CommandAllocator->Reset();
			m_CommandList->Reset(m_CommandAllocator, nullptr);
			return m_CommandList;
		}

		void ExecuteCommandList();

		inline void Flush(size_t count)
		{
			LD_CORE_TRACE("Flushing D3D12 CommandQueue");
			for (size_t i = 0; i < count; i++)
			{
				SignalAndWait();
			}
		}

		static DirectX12Renderer* Get() { return (DirectX12Renderer*)InternalRenderer::Get(); }

		inline auto* const& GetDevice() { return m_Device; }
		inline auto* const& GetCommandQueue() { return m_CommandQueue; }
		inline auto* const& GetCommandList() { return m_CommandList; }
		inline auto* const& GetDXGIFactory() { return m_DXGIFactory; }

	private:
		void ShutDown();
		void SignalAndWait();

		ID3D12Device8* m_Device = nullptr;
		ID3D12CommandQueue* m_CommandQueue = nullptr;

		ID3D12CommandAllocator* m_CommandAllocator = nullptr;
		ID3D12GraphicsCommandList6* m_CommandList = nullptr;

		IDXGIFactory7* m_DXGIFactory = nullptr;

		ID3D12Fence1* m_Fence = nullptr;
		UINT64 m_FenceValue = 0;
		HANDLE m_FenceEvent = nullptr;

#ifdef LUDO_DEBUG
		ID3D12Debug* m_D3D12Debug = nullptr;
		IDXGIDebug1* m_DXGIDebug = nullptr;
#endif 
	};

}