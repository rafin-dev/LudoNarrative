#include "ldpch.h"
#include "UniformBuffer.h"

#include "Ludo/Renderer/Renderer.h"

#ifdef LD_PLATFORM_WINDOWS

#include "Platform/DirectX12/DirectX12UniformBuffer.h"

#endif

namespace Ludo {

    Ref<UniformBuffer> UniformBuffer::Create(size_t size, uint32_t slot)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None: LD_CORE_ASSERT(false, "RenderAPI::None is currently not supported") return nullptr;
        case RendererAPI::API::DirectX12: return CreateRef<DirectX12UniformBuffer>(size, slot);
        }

        LD_CORE_ASSERT(false, "Unknown RenderAPI specified, please provide a existing RenderAPI!");
        return nullptr;
    }

}