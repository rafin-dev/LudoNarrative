#pragma once

#include "Ludo/Renderer/Buffer.h"
#include "Platform/DirectX12/Utils/DX12UploadBuffer.h"

#include "d3d12.h"

namespace Ludo {

	class DirectX12VertexBuffer : public VertexBuffer
	{
	public:
		DirectX12VertexBuffer(float* verticies, uint32_t size, const BufferLayout& layout);
		DirectX12VertexBuffer(uint32_t size, const BufferLayout& layout);
		virtual ~DirectX12VertexBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }

		virtual void SetData(void* data, uint32_t size) override;

	private:
		void ImplBind(uint32_t slot) const;

		bool Init(size_t size);
		void ShutDown();
		
		DX12UploadBuffer m_UploadBuffer;

		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView = {};
		ID3D12Resource2* m_VertexBuffer = nullptr;
		BufferLayout m_Layout;

		friend class DirectX12VertexArray;
	};

	class DirectX12IndexBuffer : public IndexBuffer
	{
	public:
		DirectX12IndexBuffer(uint32_t* indices, uint32_t count);
		bool Init(uint32_t* indices, uint32_t count);
		virtual ~DirectX12IndexBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetCount() const override;

	private:
		void ShutDown();

		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView = {};
		uint32_t m_Count;
		ID3D12Resource2* m_IndexBuffer = nullptr;
	};
}