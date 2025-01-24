#include "ldpch.h"
#include "EditorCamera.h"

#include "Ludo/Core/Application.h"
#include "Ludo/Core/Input.h"

namespace Ludo {

	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
	{
		UpdateView();
	}

	void EditorCamera::OnUpdate(TimeStep ts)
	{
		if (Input::IsKeyPressed(KeyCode::Left_ALT))
		{
			bool anyMouseButtonDown = false;
		
			const DirectX::XMFLOAT2 mouse = { Input::GetMouseX(), Input::GetMouseY() };
			DirectX::XMFLOAT2 delta = { (mouse.x - m_InitialMousePosition.x) * 0.003f, (mouse.y - m_InitialMousePosition.y) * 0.003f };
		
			if (Input::IsMouseButtonDown(MouseButtonCode::Middle))
			{
				MousePan(delta);
				anyMouseButtonDown = true;
			}
			else if (Input::IsMouseButtonDown(MouseButtonCode::Left))
			{
				MouseRotate(delta);
				anyMouseButtonDown = true;
			}
			else if (Input::IsMouseButtonDown(MouseButtonCode::Right))
			{
				MouseZoom(delta.y);
				anyMouseButtonDown = true;
			}
			
			if (anyMouseButtonDown)
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_None);
				auto& window = Application::Get().GetWindow();
				Input::SetMousePos(window.GetWidth() / 2, window.GetHeight() / 2);
			}
		}

		m_InitialMousePosition = { Input::GetMouseX(), Input::GetMouseY() };

		UpdateView();
	}

	void EditorCamera::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseScrolledEvent>(LUDO_BIND_EVENT_FN(EditorCamera::OnMouseScrolledEvent));
	}

	DirectX::XMFLOAT4X4 EditorCamera::GetViewProjection() const
	{
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, DirectX::XMLoadFloat4x4(&m_ViewMatrix) * DirectX::XMLoadFloat4x4(&m_Projection));
		return viewProjection;
	}

	DirectX::XMFLOAT3 EditorCamera::GetUpDirection() const
	{
		DirectX::XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
		auto orientation = GetOrientation();
		DirectX::XMStoreFloat3(&up, DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&up), DirectX::XMLoadFloat4(&orientation)));
		return up;
	}

	DirectX::XMFLOAT3 EditorCamera::GetRightDirection() const
	{
		DirectX::XMFLOAT3 right = { 1.0f, 0.0f, 0.0f };
		auto orientation = GetOrientation();
		DirectX::XMStoreFloat3(&right, DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&right), DirectX::XMLoadFloat4(&orientation)));
		return right;
	}

	DirectX::XMFLOAT3 EditorCamera::GetForwardDirection() const
	{
		DirectX::XMFLOAT3 forward = { 0.0f, 0.0f, -1.0f };
		auto orientation = GetOrientation();
		DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&forward), DirectX::XMLoadFloat4(&orientation)));
		return forward;
	}

	DirectX::XMFLOAT4 EditorCamera::GetOrientation() const
	{
		DirectX::XMFLOAT4 orientation;
		DirectX::XMStoreFloat4(&orientation, DirectX::XMQuaternionRotationRollPitchYaw(-m_Pitch, -m_Yaw, 0.0f));
		return orientation;
	}

	void EditorCamera::UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		DirectX::XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip));
	}

	void EditorCamera::UpdateView()
	{
		m_Position = CalCulatePosition();

		auto orientation = GetOrientation();
		DirectX::XMStoreFloat4x4(&m_ViewMatrix, DirectX::XMMatrixInverse(nullptr,
			DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&orientation)) * DirectX::XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z)));
	}

	bool EditorCamera::OnMouseScrolledEvent(MouseScrolledEvent& event)
	{
		float yOffset = event.GetYOffset();
		for (int i = 0; i < std::abs(yOffset); i++)
		{
			MouseZoom(yOffset > 0 ? 0.01f : -0.01f);
		}
		UpdateView();

		return false;
	}

	void EditorCamera::MousePan(const DirectX::XMFLOAT2& delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();

		DirectX::XMVECTOR focalPoint = DirectX::XMLoadFloat3(&m_FocalPoint);
		auto right = GetRightDirection();
		auto up = GetUpDirection();
		focalPoint = DirectX::XMVectorAdd(focalPoint, DirectX::XMVectorScale(DirectX::XMVectorScale(DirectX::XMLoadFloat3(&right), -1.0f), delta.x * xSpeed * m_Distance));
		focalPoint = DirectX::XMVectorAdd(focalPoint, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&up), delta.y * xSpeed * m_Distance));
		DirectX::XMStoreFloat3(&m_FocalPoint, focalPoint);
	}

	void EditorCamera::MouseRotate(const DirectX::XMFLOAT2& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();
	}

	void EditorCamera::MouseZoom(float delta)
	{
		m_Distance -= delta * ZoomSpeed();
		m_Distance = std::max(m_Distance, 1.0f);
	}

	DirectX::XMFLOAT3 EditorCamera::CalCulatePosition() const
	{
		DirectX::XMFLOAT3 position;
		auto forward = GetForwardDirection();
		DirectX::XMStoreFloat3(&position, 
			DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&m_FocalPoint), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&forward), -m_Distance)));
		return position;
	}

	std::pair<float, float> EditorCamera::PanSpeed() const
	{
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f);
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f);
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;
	
		return { xFactor, yFactor };
	}

	float EditorCamera::RotationSpeed() const
	{
		return 0.8f;
	}

	float EditorCamera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f);
		return speed;
	}

}