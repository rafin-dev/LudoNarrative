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

		for (int i = 0; i < m_VertexBuffers.size(); i++)
		{
			m_D3D11Buffers[i] = ((DirectX11VertexBuffer*)m_VertexBuffers[i].get())->GetBuffer();
			m_Strides[i] = m_VertexBuffers[i]->GetLayout().GetStride();
		}

		DirectX11API::Get()->GetDeviceContext()->IASetVertexBuffers(0, m_D3D11Buffers.size(), m_D3D11Buffers.data(), m_Strides.data(), m_Offsets.data());
		m_IndexBuffer->Bind();
	}

	void DirectX11VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vb)
	{
		LD_PROFILE_FUNCTION();

		m_VertexBuffers.emplace_back(vb);
		m_D3D11Buffers.emplace_back(((DirectX11VertexBuffer*)vb.get())->GetBuffer());
		m_Strides.emplace_back(vb->GetLayout().GetStride());
		m_Offsets.emplace_back(0);
	}

	void DirectX11VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& ib)
	{
		LD_PROFILE_FUNCTION();

		m_IndexBuffer = ib;
	}

}