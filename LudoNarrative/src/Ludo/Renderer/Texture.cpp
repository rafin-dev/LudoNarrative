#include "ldpch.h"
#include "Texture.h"

#include "RendererAPI.h"

#ifdef LD_PLATFORM_WINDOWS

#include "Platform/DirectX12/DirectX12Texture.h"

#endif

namespace Ludo {

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::None: LD_CORE_ASSERT(false, "None is not yet supported!"); return nullptr;
            case RendererAPI::API::DirectX12: return CreateRef<DirectX12Texture2D>(width, height);
        }

        LD_CORE_ASSERT(false, "Unknow RenderAPI!");
        return nullptr;
    }

    Ref<Texture2D> Texture2D::Create(const std::string& path)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: LD_CORE_ASSERT(false, "None is not yet supported!"); return nullptr;
        case RendererAPI::API::DirectX12: return CreateRef<DirectX12Texture2D>(path);
        }

        LD_CORE_ASSERT(false, "Unknow RenderAPI!");
        return nullptr;
    }

}