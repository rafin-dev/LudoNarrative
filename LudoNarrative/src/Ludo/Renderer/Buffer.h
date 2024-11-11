#pragma once

#include "typeinfo"

#include "Ludo/Core.h"

// IMPORTANT: Current DirectX12 Buffer(ID3D12Resource2) creation is EXTREMELY ineficient
// TODO: Fix that LOL

namespace Ludo {

	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Float3x3, Float4x4, Int, Int2, Int3, Int4, Uint, Uint2, Uint3, Uint4, Bool
	};

	static uint32_t ShaderdataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:    return 4;
			case ShaderDataType::Float2:   return 4 * 2;
			case ShaderDataType::Float3:   return 4 * 3;
			case ShaderDataType::Float4:   return 4 * 4;
			case ShaderDataType::Float3x3: return 4 * 3 * 3;
			case ShaderDataType::Float4x4: return 4 * 4 * 4;
			case ShaderDataType::Int:      return 4;
			case ShaderDataType::Int2:     return 4 * 2;
			case ShaderDataType::Int3:     return 4 * 3;
			case ShaderDataType::Int4:     return 4 * 4;
			case ShaderDataType::Uint:     return 4;
			case ShaderDataType::Uint2:    return 4 * 2;
			case ShaderDataType::Uint3:    return 4 * 3;
			case ShaderDataType::Uint4:    return 4 * 4;
			case ShaderDataType::Bool:     return 1;
		}

		LD_CORE_ASSERT(false, "Unknown Shader Data Type");
		return 0;
	}

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		BufferElement(const std::string& name, ShaderDataType type, bool normal = false)
			: Name(name), Type(type), Size(ShaderdataTypeSize(type)), Offset(0), Normalized(normal)
		{
		}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case ShaderDataType::Float:    return 1;
				case ShaderDataType::Float2:   return 2;
				case ShaderDataType::Float3:   return 3;
				case ShaderDataType::Float4:   return 4;
				case ShaderDataType::Float3x3: return 3 * 3;
				case ShaderDataType::Float4x4: return 4 * 4;
				case ShaderDataType::Int:      return 1;
				case ShaderDataType::Int2:     return 2;
				case ShaderDataType::Int3:     return 3;
				case ShaderDataType::Int4:     return 4;
				case ShaderDataType::Uint2:    return 2;
				case ShaderDataType::Uint3:    return 3;
				case ShaderDataType::Uint4:    return 4;
				case ShaderDataType::Bool:     return 1;
			}

			LD_CORE_ASSERT(false, "Unknow Type");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalCulateOffSetAndStride();
		}
		BufferLayout() = default;

		inline uint32_t GetStride() const { return m_Stride; }
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalCulateOffSetAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;

				m_Stride += element.Size;
			}
		}

		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;
	
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const BufferLayout& GetLayout() const = 0;

		static VertexBuffer* Create(float* vertices, uint32_t size, const BufferLayout& layout);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static IndexBuffer* Create(uint32_t* indices, uint32_t count);
	};

}