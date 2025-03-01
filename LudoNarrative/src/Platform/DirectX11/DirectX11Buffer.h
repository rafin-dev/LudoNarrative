#pragma once

#include "Ludo/Renderer/Buffer.h"

#include <d3d11.h>

namespace Ludo {

	class DirectX11VertexBuffer : public VertexBuffer
	{
	public:
		DirectX11VertexBuffer(float* verticies, uint32_t size, const BufferLayout& layout, VBUpdateFrequency frequency);
		DirectX11VertexBuffer(uint32_t size, const BufferLayout& layout, VBUpdateFrequency frequency);
		bool Init(uint32_t size);
		virtual ~DirectX11VertexBuffer() override;

		void Bind() const override;
		void Unbind() const override;

		void BindImpl(uint32_t slot) const;

		const BufferLayout& GetLayout() const override { return m_Layout; }

		void SetData(void* data, uint32_t size) override;

		ID3D11Buffer*& const GetBuffer() { return m_VertexBuffer; }

	private:
		void ShutDown();

		ID3D11Buffer* m_VertexBuffer = nullptr;

		BufferLayout m_Layout;
		uint32_t m_Size;
	};

	class DirectX11IndexBuffer : public IndexBuffer
	{
	public:
		DirectX11IndexBuffer(uint32_t* indices, uint32_t count);
		bool Init(uint32_t* indices, uint32_t count);
		virtual ~DirectX11IndexBuffer() override;

		void Bind() const override;
		void Unbind() const override;

		uint32_t GetCount() const { return m_Count; }

	private:
		void ShutDown();

		ID3D11Buffer* m_IndexBuffer = nullptr;
		uint32_t m_Count = 0;
	};

}