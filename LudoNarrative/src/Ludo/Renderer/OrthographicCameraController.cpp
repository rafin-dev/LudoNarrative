#include "ldpch.h"
#include "OrthographicCameraController.h"

#include "Ludo/Core/Input.h"
#include "Ludo/Core/KeyCodes.h"

namespace Ludo {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotationEnabled)
		: m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_RotationEnabled(rotationEnabled)
	{
	}

	void OrthographicCameraController::OnUpdate(TimeStep timeStep)
	{
		LD_PROFILE_FUNCTION();

		m_CameraPosition.x += (Input::IsKeyPressed(KeyCode::D) - Input::IsKeyPressed(KeyCode::A)) * m_ZoomLevel * timeStep;
		m_CameraPosition.y += (Input::IsKeyPressed(KeyCode::W) - Input::IsKeyPressed(KeyCode::S)) * m_ZoomLevel * timeStep;

		if (m_RotationEnabled)
		{
			int rotationDirection = Ludo::Input::IsKeyPressed(KeyCode::E) - Ludo::Input::IsKeyPressed(KeyCode::Q);
			m_CameraRotation += rotationDirection * m_CameraRotationSpeed * timeStep;
			m_Camera.SetRotation(m_CameraRotation);
		}

		m_Camera.SetPosition(m_CameraPosition);
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		LD_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(LUDO_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolledEvent));
	}

	void OrthographicCameraController::CalculateView()
	{
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	bool OrthographicCameraController::OnMouseScrolledEvent(MouseScrolledEvent& e)
	{
		LD_PROFILE_FUNCTION();

		m_ZoomLevel -= (float)e.GetYOffset() * 0.0025f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);

		CalculateView();

		return false;
	}

}