#pragma once

#include "typeinfo"

// IMPORTANT: Current DirectX12 Buffer(ID3D12Resource2) creation is EXTREMELY ineficient
// TODO: Fix that LOL

namespace Ludo {

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;
	
		virtual void Bind() const = 0;

		static VertexBuffer* Create(float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static IndexBuffer* Create(uint32_t* indices, uint32_t count);
	};

}