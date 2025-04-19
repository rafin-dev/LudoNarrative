#pragma once

#include "ViewBase.h"

#include "misc/OpenAndSelectedsManager.h"

#include <LudoNarrative.h>

namespace Ludo {

	class SceneViewerView : public ProjectEditorViewBase
	{
	public:
		SceneViewerView(const Ref<OpenAndSelectedsManager>& oas);
		~SceneViewerView() override;

		void OnUpdate(TimeStep ts) override;
		void OnImGuiRender() override;

		bool OnEvent(Event& event) override;

	private:
		bool OnKeyPressed(KeyPressedEvent& event);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& event);

		void OnOverlayRender();

		Ref<OpenAndSelectedsManager> m_OpenAndSelecteds;

		Ref<FrameBuffer> m_FrameBuffer;

		EditorCamera m_EditorCamera;

		bool m_ViewportActive = false;
		DirectX::XMFLOAT2 m_ViewportSize = { 0.0f, 0.0f };
		bool m_ResizeFrameBuffer = false;
		DirectX::XMFLOAT2 m_MinViewportBounds = {};
		uint32_t MouseX = 0, MouseY = 0;

		int m_GizmoType = 0;
		bool m_GizmoHovered = false;

		bool m_ShowPhysicsColliders = true;

		enum class SceneState
		{
			Edit = 0,
			Play = 1,
			Simulate = 2
		};

		SceneState m_SceneState = SceneState::Edit;
	};

}