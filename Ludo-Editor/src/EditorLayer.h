#pragma once

#include "Panels/ContentBrowserPanel.h"

#include "EditorPanels/SceneEditorPanels/ScenePanelHierarchy.h"

#include <LudoNarrative.h>

#include <filesystem>

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
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& event);

		void OnOverlayRender();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();

		void SetImGuiDarkTheme();

		void OnScenePlay();
		void OnSceneStop();

		void OnSceneSimulateStart();
		void OnSceneSimulateStop();

		void OnDuplicateEntity();

		// UI Toolbar
		void RenderToolBar();

		Ref<ImGuiTexture> m_PlayButtonIcon;
		Ref<ImGuiTexture> m_StopButtonIcon;

		EditorCamera m_EditorCamera;

		Ref<FrameBuffer> m_FrameBuffer;
		Ref<Texture2D> m_Texture;
	
		std::filesystem::path m_EditorScenePath;
		std::vector<Ref<Scene>> m_OpenScenes;
		std::vector<std::filesystem::path> m_ScenePaths;
		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;

		std::string m_NewSceneName;

		bool m_ViewportActive = false;
		DirectX::XMFLOAT2 m_ViewportSize = { 0.0f, 0.0f };
		bool m_ResizeFrameBuffer = false;
		DirectX::XMFLOAT2 m_MinViewportBounds;
		int MouseX = 0, MouseY = 0;

		int m_GizmoType = -1;
		bool m_GizmoHovered = false;

		bool m_ShowPhysicsColliders = true;

		// Panels
		ScenePanelHierarchy m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		enum class SceneState
		{
			Edit = 0,
			Play = 1,
			Simulate = 2
		};

		SceneState m_SceneState = SceneState::Edit;
	};

}