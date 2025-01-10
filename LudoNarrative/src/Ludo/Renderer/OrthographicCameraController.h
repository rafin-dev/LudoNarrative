#pragma once

#include "Ludo/Core/Core.h"
#include "Ludo/Core/TimeStep.h"
#include "Ludo/Renderer/OrthographicCamera.h"

#include "Ludo/Events/ApplicationEvent.h"
#include "Ludo/Events/MouseEvent.h"

namespace Ludo {

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotationEnabled = false);

		void OnUpdate(TimeStep timeStep);
		void OnEvent(Event& e);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }

		float GetAspectRatio() const { return m_AspectRatio; }
		void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; CalculateView(); }

		float GetZoomLevel() const { return m_ZoomLevel; }
		void SetZoomLevel(float zommLevel) { m_ZoomLevel = zommLevel; CalculateView(); }

	private:
		void CalculateView();

		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);

		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;

		bool m_RotationEnabled;
		
		DirectX::XMFLOAT3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f;

		float m_CameraRotationSpeed = 1.0f;
	};

}