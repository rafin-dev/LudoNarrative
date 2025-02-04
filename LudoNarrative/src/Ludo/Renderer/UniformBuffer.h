#pragma once

#include "Ludo/Core/Core.h"

namespace Ludo {

	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() = default;

		virtual void SetData(void* data, size_t size, size_t offset) = 0;

		virtual void Bind() const = 0;

		static Ref<UniformBuffer> Create(size_t size, uint32_t slot);
	};

}