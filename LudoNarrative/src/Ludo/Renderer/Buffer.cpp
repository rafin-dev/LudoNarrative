#include "ldpch.h"
#include "Buffer.h"

#include "Ludo/Core/Core.h"
#include "Renderer.h"

#ifdef LD_PLATFORM_WINDOWS
    #include "Platform/DirectX12/DirectX12Buffer.h"
#endif 

namespace Ludo {

    Ref<VertexBuffer> VertexBuffer::Create(uint32_t size, const BufferLayout& layout, VBUpdateFrequency updateFrequency)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None: LD_CORE_ASSERT(false, "RenderAPI::None is currently not supported") return nullptr;
        case RendererAPI::API::DirectX12: return CreateRef<DirectX12VertexBuffer>(size, layout, updateFrequency);
        }

        LD_CORE_ASSERT(false, "Unknown RenderAPI specified, please provide a existing RenderAPI!");
        return nullptr;
    }

    Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size, const BufferLayout& layout, VBUpdateFrequency updateFrequency)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None: LD_CORE_ASSERT(false, "RenderAPI::None is currently not supported") return nullptr;
            case RendererAPI::API::DirectX12: return CreateRef<DirectX12VertexBuffer>(vertices, size, layout, updateFrequency);
        }

        LD_CORE_ASSERT(false, "Unknown RenderAPI specified, please provide a existing RenderAPI!");
        return nullptr;
    }

    Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None: LD_CORE_ASSERT(false, "RenderAPI::None is currently not supported") return nullptr;
            case RendererAPI::API::DirectX12: return CreateRef<DirectX12IndexBuffer>(indices, count);
        }

        LD_CORE_ASSERT(false, "Unknown RenderAPI specified, please provide a existing RenderAPI!");
        return nullptr;
    }

}