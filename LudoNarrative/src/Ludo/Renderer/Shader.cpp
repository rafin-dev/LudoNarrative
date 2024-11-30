#include "ldpch.h"
#include "Shader.h"

#include "Ludo/Core.h"
#include "Renderer.h"

#ifdef LD_PLATFORM_WINDOWS
    #include "Platform/DirectX11/DirectX11Shader.h"
#endif 

namespace Ludo {

    Shader* Shader::Create(const LUDO_SHADER_DESC& desc)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None: LD_CORE_ASSERT(false, "RenderAPI::None is currently not supported") return nullptr;
            case RendererAPI::API::DirectX11: return new DirectX11Shader(desc);
        }

        LD_CORE_ASSERT(false, "Unknown RenderAPI specified, please provide a existing RenderAPI!");
        return nullptr;
    }

}