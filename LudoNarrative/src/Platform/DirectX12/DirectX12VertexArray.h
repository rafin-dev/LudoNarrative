#pragma once

#include "Ludo/Renderer/VertexArray.h"

namespace Ludo {

	class DirectX12VertexArray : public VertexArray
	{
	public:
		DirectX12VertexArray();
		virtual ~DirectX12VertexArray() override;

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