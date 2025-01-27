#pragma once

#include "Ludo/Core/TimeStep.h"
#include "Ludo/Events/MouseEvent.h"
#include "Ludo/Events/KeyEvent.h"
#include "Ludo/Renderer/Camera.h"

#include <DirectXMath.h>

#include <imgui.h>

namespace Ludo {

	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(TimeStep ts);
		void OnEvent(Event& event);

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

		inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

		const DirectX::XMFLOAT4X4& GetViewMatrix() const { return m_ViewMatrix; }
		DirectX::XMFLOAT4X4 GetViewProjection() const;

		DirectX::XMFLOAT3 GetUpDirection() const;
		DirectX::XMFLOAT3 GetRightDirection() const;
		DirectX::XMFLOAT3 GetForwardDirection() const;
		const DirectX::XMFLOAT3& GetPosition() const { return m_Position; }
		DirectX::XMFLOAT4 GetOrientation() const;

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }

	private:
		void UpdateProjection();
		void UpdateView();

		bool OnMouseScrolledEvent(MouseScrolledEvent& event);

		void MousePan(const DirectX::XMFLOAT2& delta);
		void MouseRotate(const DirectX::XMFLOAT2& delta);
		void MouseZoom(float delta);

		DirectX::XMFLOAT3 CalCulatePosition() const;

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;

		float m_FOV = 45.0f;
		float m_AspectRatio = 1.0f;
		float m_NearClip = 0.1f;
		float m_FarClip = 1000.0f;

		DirectX::XMFLOAT4X4 m_ViewMatrix;
		DirectX::XMFLOAT3 m_Position = { 0.0f, 0.0f, -10.0f };
		DirectX::XMFLOAT3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		DirectX::XMFLOAT2 m_InitialMousePosition = { 0.0f, 0.0f };

		float m_Distance = 10.0f;
		float m_Pitch = 0.0f;
		float m_Yaw = 0.0f;

		float m_ViewportWidth = 1280;
		float m_ViewportHeight = 720;
	};

}