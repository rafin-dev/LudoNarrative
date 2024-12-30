#include "ldpch.h"
#include "VertexArray.h"

#include "Ludo/Renderer/RendererAPI.h"

#ifdef LD_PLATFORM_WINDOWS

#include "Platform/DirectX12/DirectX12VertexArray.h"

#endif

namespace Ludo {

    Ref<VertexArray> VertexArray::Create()
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::None: LD_CORE_ASSERT(false, "None is not yet supported") return nullptr;
            case RendererAPI::API::DirectX12: return CreateRef<DirectX12VertexArray>();
        }

        return nullptr;
    }

}