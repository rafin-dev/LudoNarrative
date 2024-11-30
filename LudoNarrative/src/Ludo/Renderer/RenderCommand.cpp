#include "ldpch.h"
#include "RenderCommand.h"

#ifdef LD_PLATFORM_WINDOWS
	#include "Platform/DirectX11/DirectX11API.h"
#endif 

#include "Ludo/Core.h"

namespace Ludo {

	static RendererAPI* GetRenderAPI()
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None: LD_CORE_ASSERT(false, "None is not yet supported!") return nullptr;
			case RendererAPI::API::DirectX11: return DirectX11API::Get();
		}
	}

	RendererAPI* RenderCommand::s_RenderAPI = GetRenderAPI();

}