#pragma once

#include "Ludo/Renderer/Buffer.h"

namespace Ludo {

	class DirectX12VertexBuffer : public VertexBuffer
	{
	public:
		DirectX12VertexBuffer(float* verticies, uint32_t size);
		bool Init(float* verticies, uint32_t size);
		virtual ~DirectX12VertexBuffer() override;

		virtual void Bind() const override;

	private:
		void ShutDown();

		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView = {};
		ID3D12Resource2* m_VertexBuffer = nullptr;
	};

	class DirectX12IndexBuffer : public IndexBuffer
	{
	public:
		DirectX12IndexBuffer(uint32_t* indices, uint32_t size);
		bool Init(uint32_t* indices, uint32_t count);
		virtual ~DirectX12IndexBuffer() override;

		virtual void Bind() const override;

		virtual uint32_t GetCount() const override;

	private:
		void ShutDown();

		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView = {};
		uint32_t m_Count;
		ID3D12Resource2* m_IndexBuffer = nullptr;
	};

}