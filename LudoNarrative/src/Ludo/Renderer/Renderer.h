#pragma once

#include "RenderCommand.h"
#include "Renderer2D.h"
#include "Shader.h"
#include "Material.h"

#include "OrthographicCamera.h"

namespace Ludo {

	class Renderer
	{
	public:
		static inline bool Init()
		{ 
			LD_PROFILE_FUNCTION();

			bool Succes = RenderCommand::Init();
			if (Succes)
			{
				Renderer2D::Init();
			}
			return Succes;
		}

		static inline void ShutDown() 
		{ 
			LD_PROFILE_FUNCTION();

			Renderer2D::Shutdown();
			RenderCommand::ShutDown(); 
		}

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Material>& material, 
			const Ref<VertexArray>& vertexArray, 
			const DirectX::XMFLOAT4X4& transform);

		static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			DirectX::XMFLOAT4X4 ViewProjectionMatrix;
		};

		static SceneData* s_SceneData;
	};

}