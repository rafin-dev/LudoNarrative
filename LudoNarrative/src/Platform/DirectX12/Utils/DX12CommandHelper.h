#pragma once

#include <d3d12.h>
#include <cstdint>

namespace Ludo {

	class DX12CommandHelper
	{
	public:
		bool Init();
		void ShutDown();

		ID3D12GraphicsCommandList6* const& InitCommandList();

		void Flush(uint32_t count);

		void ExecuteCommandListAndWait();

		inline ID3D12CommandQueue* const& GetCommandQueue() { return m_CommandQueue; };
		inline ID3D12GraphicsCommandList6* const& GetCommandList() { return m_CommandList; };

	private:
		void SignalAndWait();

		ID3D12CommandQueue* m_CommandQueue = nullptr;
		ID3D12CommandAllocator* m_CommandAllocator = nullptr;
		ID3D12GraphicsCommandList6* m_CommandList = nullptr;

		ID3D12Fence1* m_Fence = nullptr;
		UINT64 m_FenceValue = 0;
		HANDLE m_FenceEvent = nullptr;
	};

}