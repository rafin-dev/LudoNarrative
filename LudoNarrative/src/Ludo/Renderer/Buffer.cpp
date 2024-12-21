#include "ldpch.h"
#include "Buffer.h"

#include "Ludo/Core/Core.h"
#include "Renderer.h"

#ifdef LD_PLATFORM_WINDOWS
    #include "Platform/DirectX11/DirectX11Buffer.h"
#endif 

namespace Ludo {

    Ref<VertexBuffer> VertexBuffer::Create(uint32_t size, const BufferLayout& layout)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None: LD_CORE_ASSERT(false, "RenderAPI::None is currently not supported") return nullptr;
        case RendererAPI::API::DirectX11: return CreateRef<DirectX11VertexBuffer>(size, layout);
        }

        LD_CORE_ASSERT(false, "Unknown RenderAPI specified, please provide a existing RenderAPI!");
        return nullptr;
    }

    Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size, const BufferLayout& layout)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None: LD_CORE_ASSERT(false, "RenderAPI::None is currently not supported") return nullptr;
            case RendererAPI::API::DirectX11: return CreateRef<DirectX11VertexBuffer>(vertices, size, layout);
        }

        LD_CORE_ASSERT(false, "Unknown RenderAPI specified, please provide a existing RenderAPI!");
        return nullptr;
    }

    Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None: LD_CORE_ASSERT(false, "RenderAPI::None is currently not supported") return nullptr;
            case RendererAPI::API::DirectX11: return CreateRef<DirectX11IndexBuffer>(indices, count);
        }

        LD_CORE_ASSERT(false, "Unknown RenderAPI specified, please provide a existing RenderAPI!");
        return nullptr;
    }

}