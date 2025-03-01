#pragma once

#include "Ludo/Renderer/VertexArray.h"
#include "DirectX11Buffer.h"

#include "d3d11.h"

namespace Ludo {

	class DirectX11VertexArray : public VertexArray
	{
	public:
		DirectX11VertexArray();
		virtual ~DirectX11VertexArray() override;

		virtual void Bind() override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vb) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& ib) override;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() override { return m_VertexBuffers; }
		virtual const Ref<IndexBuffer>& GetIndexBuffer() override { return m_IndexBuffer; }

	private:
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
	};

}