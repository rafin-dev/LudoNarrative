#include "ldpch.h"
#include "ImGuiTexture.h"

#include "Ludo/Renderer/RendererAPI.h"

#ifdef LD_PLATFORM_WINDOWS

#include "Platform/DirectX12/DirectX12ImGuiTexture.h"

#endif

namespace Ludo {

    Ref<ImGuiTexture> ImGuiTexture::Create(const Ref<Texture2D>& texture)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: LD_CORE_ASSERT(false, "None is not yet supported"); return nullptr;
        case RendererAPI::API::DirectX12: return CreateRef<DirectX12ImGuiTexture>(texture);
        }

        LD_CORE_ASSERT(false, "Unknown Render API");
        return nullptr;
    }

    Ref<ImGuiTexture> ImGuiTexture::Create(const Ref<SubTexture2D>& texture)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None: LD_CORE_ASSERT(false, "None is not yet supported"); return nullptr;
        case RendererAPI::API::DirectX12: return CreateRef<DirectX12ImGuiTexture>(texture);
        }

        LD_CORE_ASSERT(false, "Unknown Render API");
        return nullptr;
    }

}