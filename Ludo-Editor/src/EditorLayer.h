#pragma once

#include <LudoNarrative.h>

namespace Ludo {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		void OnUpdate(TimeStep ts) override;
		void OnEvent(Event& event) override;

		void OnImGuiRender() override;

	private:
		Ref<FrameBuffer> m_FrameBuffer;
		Ref<Texture2D> m_Texture;
	
		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;
		Entity m_CameraEntity;

		bool m_ViewportFocused = false;
		DirectX::XMFLOAT2 m_ViewportSize = { 0.0f, 0.0f };
		bool m_ResizeFrameBuffer = false;
	};

}