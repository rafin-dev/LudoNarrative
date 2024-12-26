#include "ldpch.h"
#include "RenderCommand.h"

#ifdef LD_PLATFORM_WINDOWS

#include "Platform/DirectX11/DirectX11API.h"
#include "Platform/DirectX12/DirectX12API.h"

#endif 

#include "Ludo/Core/Core.h"

namespace Ludo {

	static RendererAPI* GetRenderAPI()
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None: LD_CORE_ASSERT(false, "None is not yet supported!") return nullptr;
			case RendererAPI::API::DirectX11: return DirectX11API::Get();
			case RendererAPI::API::DirectX12: return DirectX12API::Get();
		}
	}

	RendererAPI* RenderCommand::s_RenderAPI = GetRenderAPI();

}