#include "ldpch.h"
#include "Texture.h"

#include "RendererAPI.h"

#ifdef LD_PLATFORM_WINDOWS

#include "Platform/DirectX11/DirectX11Texture.h"

#endif

namespace Ludo {

    Ref<Texture2D> Texture2D::Create(const std::string& path)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: LD_CORE_ASSERT(false, "None is not yet supported!"); return nullptr;
        case RendererAPI::API::DirectX11: return Ref<Texture2D>(new DirectX11Texture2D(path));
        }

        LD_CORE_ASSERT(false, "Unknow RenderAPI!");
        return nullptr;
    }

}