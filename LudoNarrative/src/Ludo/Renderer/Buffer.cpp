#include "ldpch.h"
#include "Buffer.h"

#include "Ludo/Core.h"
#include "Renderer.h"

#ifdef LD_PLATFORM_WINDOWS
    #include "Platform/DirectX12/DirectX12Buffer.h"
#endif 

namespace Ludo {

    VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::None: LD_CORE_ASSERT(false, "RenderAPI::None is currently not supported") return nullptr;
            case RendererAPI::DirectX12: return new DirectX12VertexBuffer(vertices, size);
        }

        LD_CORE_ASSERT(false, "Unknown RenderAPI specified, please provide a existing RenderAPI!");
        return nullptr;
    }

    IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::None: LD_CORE_ASSERT(false, "RenderAPI::None is currently not supported") return nullptr;
            case RendererAPI::DirectX12: return new DirectX12IndexBuffer(indices, count * sizeof(uint32_t));
        }

        LD_CORE_ASSERT(false, "Unknown RenderAPI specified, please provide a existing RenderAPI!");
        return nullptr;
    }

}