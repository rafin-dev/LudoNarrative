#pragma once

#include <LudoNarrative.h>

#include "Panels/SceneHierarchyPanel.h"

namespace Ludo {

	class EditorLayer : public Layer
	{
	public:
		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate(TimeStep ts) override;
		void OnEvent(Event& event) override;

		void OnImGuiRender() override;

	private:
		void SetImGuiDarkTheme();

		Ref<FrameBuffer> m_FrameBuffer;
		Ref<Texture2D> m_Texture;
	
		Ref<Scene> m_ActiveScene;

		bool m_ViewportFocused = false;
		DirectX::XMFLOAT2 m_ViewportSize = { 0.0f, 0.0f };
		bool m_ResizeFrameBuffer = false;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};

}