#pragma once

#include <d3d12.h>

namespace Ludo {

	class DX12UploadBuffer
	{
	public:
		DX12UploadBuffer() = default;
		~DX12UploadBuffer();

		bool Init(size_t size);

		void ImediateUploadData(ID3D12Resource2* &const destBuffer, void* data, size_t offset, size_t size);
		void QueuUploadData(ID3D12Resource2*& const destBuffer, void* data, size_t offset, size_t size);

		void FillBufferData(void* data, size_t offset, uint32_t size);

		bool Resize(size_t size);

		ID3D12Resource2*& const GetBuffer() { return m_UploadBuffer; }
		size_t GetSize() { return m_Size; }

	private:
		void ShutDown();

		bool CreateUploadBuffer();

		ID3D12Resource2* m_UploadBuffer = nullptr;
		size_t m_Size = 0;
		void* m_Dest = nullptr;
	};

}