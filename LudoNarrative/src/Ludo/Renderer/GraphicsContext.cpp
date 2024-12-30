#include "ldpch.h"
#include "GraphicsContext.h"

#include "RendererAPI.h"

#ifdef LD_PLATFORM_WINDOWS
    #include "Platform/DirectX12/DirectX12Context.h"
    #include "Platform/Windows/WindowsWindow.h"
#endif 


namespace Ludo {

    GraphicsContext* GraphicsContext::Create(const Window* window)
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::None: LD_CORE_ASSERT(false, "None is not yet supported!"); return nullptr;
            case RendererAPI::API::DirectX12: return new DirectX12Context(((WindowsWindow*)window)->GetHandle());
        }

        LD_CORE_ASSERT(false, "Unknow RenderAPI specified");

        return nullptr;
    }

}