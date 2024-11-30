#include "ldpch.h"
#include "GraphicsContext.h"

#include "RendererAPI.h"

#ifdef LD_PLATFORM_WINDOWS
    #include "Platform/DirectX11/DirectX11Context.h"
    #include "Platform/Windows/WindowsWindow.h"
#endif 


namespace Ludo {

    GraphicsContext* GraphicsContext::Create(const Window* window)
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::None: LD_CORE_ASSERT(false, "None is not yet supported!"); return nullptr;
            case RendererAPI::API::DirectX11: return new DirectX11Context(((WindowsWindow*)window)->GetHandle());
        }

        LD_CORE_ASSERT(false, "Unknow RenderAPI specified");

        return nullptr;
    }

}