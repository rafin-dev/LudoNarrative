#pragma once

#include "Ludo/Core/Core.h"
#include "Ludo/Renderer/Buffer.h"

namespace Ludo {

	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind() = 0;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vb) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& ib) = 0;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() = 0;

		static Ref<VertexArray> Create();
	};

}