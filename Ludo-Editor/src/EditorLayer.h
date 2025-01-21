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
		bool OnKeyPressed(KeyPressedEvent& event);

		void NewScene();
		void OpenScene();
		void SaveSceneAs();

		void SetImGuiDarkTheme();

		EditorCamera m_EditorCamera;

		Ref<FrameBuffer> m_FrameBuffer;
		Ref<Texture2D> m_Texture;
	
		Ref<Scene> m_ActiveScene;

		bool m_ViewportActive = false;
		DirectX::XMFLOAT2 m_ViewportSize = { 0.0f, 0.0f };
		bool m_ResizeFrameBuffer = false;

		int m_GizmoType = -1;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};

}