#pragma once

#include "imgui/imgui.h"

namespace Ludo {

	struct FrameBufferSpecification
	{
		uint32_t Width;
		uint32_t Height;
		uint32_t Samples;

		bool SwapChainTarget = false;
	};

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual const FrameBufferSpecification& GetSpecification() const = 0;
	
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual ImTextureID GetImTextureID() const = 0;

		static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
	};

}