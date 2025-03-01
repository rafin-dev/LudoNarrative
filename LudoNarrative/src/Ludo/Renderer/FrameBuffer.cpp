#include "ldpch.h"
#include "FrameBuffer.h"

#include "Ludo/Renderer/RendererAPI.h"

#ifdef LD_PLATFORM_WINDOWS

#include "Platform/DirectX11/DirectX11FrameBuffer.h"

#endif

namespace Ludo {

	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
	{
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: LD_CORE_ASSERT(false, "RenderAPI::None is currently not supported") return nullptr;
        case RendererAPI::API::DirectX11: return CreateRef<DirectX11FrameBuffer>(spec);
        }

        LD_CORE_ASSERT(false, "Unknown RenderAPI specified, please provide a existing RenderAPI!");
        return nullptr;
	}

}