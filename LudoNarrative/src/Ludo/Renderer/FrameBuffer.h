#pragma once

#include <imgui.h>

namespace Ludo {

	enum class FrameBufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RED_INTEGER,

		// Depth/Stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	struct FrameBufferTextureSpecification
	{
		FrameBufferTextureSpecification() = default;
		FrameBufferTextureSpecification(FrameBufferTextureFormat format)
			: TextureFormat(format) { }
		FrameBufferTextureSpecification(FrameBufferTextureFormat format, bool readBack)
			: TextureFormat(format), AllowReadBack(readBack) { }
		FrameBufferTextureSpecification(FrameBufferTextureFormat format, bool readBack, DirectX::XMFLOAT4& clearColor)
			: TextureFormat(format), AllowReadBack(readBack), ClearColor(clearColor) { }

		FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat::None;
		bool AllowReadBack = false;
		DirectX::XMFLOAT4 ClearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	};

	struct FrameBufferAttachmentSpecification
	{
		FrameBufferAttachmentSpecification() = default;
		FrameBufferAttachmentSpecification(std::initializer_list<FrameBufferTextureSpecification> attachments)
			: Attachments(attachments) { }

		std::vector<FrameBufferTextureSpecification> Attachments;
	};

	struct FrameBufferSpecification
	{
		uint32_t Width;
		uint32_t Height;
		FrameBufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual const FrameBufferSpecification& GetSpecification() const = 0;
	
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, uint32_t x, uint32_t y) = 0;

		virtual ImTextureID GetImTextureID(uint32_t index = 0) = 0;

		static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
	};

}