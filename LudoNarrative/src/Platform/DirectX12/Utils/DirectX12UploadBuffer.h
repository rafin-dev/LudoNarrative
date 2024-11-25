#pragma once

#include "ldpch.h"

namespace Ludo {

	class DirectX12UploadBuffer
	{
	public:
		DirectX12UploadBuffer() = default;
		DirectX12UploadBuffer(size_t size);
		~DirectX12UploadBuffer();

		bool UploadData(ID3D12Resource2*& const destBuffer, void* data);
		bool Resize(size_t newSize);

		ID3D12Resource2*& const GetBuffer() { return m_UploadBuffer; }

	private:
		bool CreateUploadBuffer();
		void ShutDown();

		ID3D12Resource2* m_UploadBuffer = nullptr;
		size_t m_Size = 0;
		void* m_Dest = nullptr;
	};

}