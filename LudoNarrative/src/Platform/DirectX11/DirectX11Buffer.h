#pragma once

#include "Ludo/Renderer/Buffer.h"

namespace Ludo {

	class DirectX11VertexBuffer : public VertexBuffer
	{
	public:
		DirectX11VertexBuffer(float* verticies, uint32_t size, const BufferLayout& layout);
		bool Init(float* verticies, uint32_t size);
		virtual ~DirectX11VertexBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }

	private:
		void ShutDown();

		ID3D11Buffer* m_VertexBuffer = nullptr;

		BufferLayout m_Layout;
	};

	class DirectX11IndexBuffer : public IndexBuffer
	{
	public:
		DirectX11IndexBuffer(uint32_t* indices, uint32_t count);
		bool Init(uint32_t* indices, uint32_t count);
		virtual ~DirectX11IndexBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetCount() const { return m_Count; }

	private:
		void ShutDown();

		ID3D11Buffer* m_IndexBuffer = nullptr;
		uint32_t m_Count = 0;
	};

}