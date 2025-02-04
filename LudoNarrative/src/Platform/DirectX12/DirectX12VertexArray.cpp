#include "ldpch.h"
#include "DirectX12VertexArray.h"

#include "Platform/DirectX12/DirectX12Buffer.h"

namespace Ludo {

	DirectX12VertexArray::DirectX12VertexArray()
	{
		m_VertexBuffers.reserve(1);
	}

	DirectX12VertexArray::~DirectX12VertexArray()
	{
	}

	void DirectX12VertexArray::Bind()
	{
		LD_PROFILE_RENDERER_FUNCTION();

		for (int i = 0; i < m_VertexBuffers.size(); i++)
		{
			((DirectX12VertexBuffer*)m_VertexBuffers[i].get())->ImplBind(i);
		}

		if (m_IndexBuffer)
		{
			m_IndexBuffer->Bind();
		}
	}

	void DirectX12VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vb)
	{
		m_VertexBuffers.push_back(vb);
	}

	void DirectX12VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& ib)
	{
		m_IndexBuffer = ib;
	}

}