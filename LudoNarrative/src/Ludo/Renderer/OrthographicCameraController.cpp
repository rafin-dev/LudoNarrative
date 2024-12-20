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

		m_CameraPosition.x += (Input::IsKeyPressed(LD_KEY_D) - Input::IsKeyPressed(LD_KEY_A)) * m_CameraMoveSeed * timeStep;
		m_CameraPosition.y += (Input::IsKeyPressed(LD_KEY_W) - Input::IsKeyPressed(LD_KEY_S)) * m_CameraMoveSeed * timeStep;

		if (m_RotationEnabled)
		{
			int rotationDirection = Ludo::Input::IsKeyPressed(LD_KEY_E) - Ludo::Input::IsKeyPressed(LD_KEY_Q);
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
		dispatcher.Dispatch<WindowResizeEvent>(LUDO_BIND_EVENT_FN(OrthographicCameraController::OnWindowResizeEvent));
	}

	bool OrthographicCameraController::OnMouseScrolledEvent(MouseScrolledEvent& e)
	{
		LD_PROFILE_FUNCTION();

		m_ZoomLevel -= (float)e.GetYOffset() * 0.0025f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);

		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

		m_CameraMoveSeed = m_ZoomLevel;

		return false;
	}

	bool OrthographicCameraController::OnWindowResizeEvent(WindowResizeEvent& e)
	{
		LD_PROFILE_FUNCTION();

		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		LD_CORE_TRACE("New aspect ratio: {0}", m_AspectRatio);

		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

		return false;
	}

}