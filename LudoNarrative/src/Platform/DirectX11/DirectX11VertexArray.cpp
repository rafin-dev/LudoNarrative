#include "ldpch.h"
#include "DirectX11VertexArray.h"

#include "DirectX11API.h"

namespace Ludo {

	DirectX11VertexArray::DirectX11VertexArray()
	{
		LD_PROFILE_FUNCTION();

		LD_CORE_TRACE("Created Vertex Array");
	}

	DirectX11VertexArray::~DirectX11VertexArray()
	{
		LD_PROFILE_FUNCTION();

		LD_CORE_TRACE("Deleted Vertex Array with {0} Vertex Buffer(s)", m_VertexBuffers.size());
	}

	void DirectX11VertexArray::Bind()
	{
		LD_PROFILE_FUNCTION();

		for (uint32_t i = 0; i < m_VertexBuffers.size(); i++)
		{
			((DirectX11VertexBuffer*)m_VertexBuffers[i].get())->BindImpl(i);
		}

		m_IndexBuffer->Bind();
	}

	void DirectX11VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vb)
	{
		LD_PROFILE_FUNCTION();

		m_VertexBuffers.emplace_back(vb);
	}

	void DirectX11VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& ib)
	{
		LD_PROFILE_FUNCTION();

		m_IndexBuffer = ib;
	}

}