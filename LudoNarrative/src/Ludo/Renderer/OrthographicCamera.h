#pragma once

#include "ldpch.h"

namespace Ludo {

	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		inline const DirectX::XMFLOAT3& GetPosition() const { return m_Position; }
		inline void SetPosition(const DirectX::XMFLOAT3& pos) { m_Position = pos; RecalculateViewMatrix(); }

		inline float GetRotation() const { return m_Rotation; }
		inline void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

		inline const DirectX::XMFLOAT4X4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		inline const DirectX::XMFLOAT4X4& GetViewMatrix() const { return m_ViewMatrix; }
		inline const DirectX::XMFLOAT4X4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

	private:
		void RecalculateViewMatrix();

		DirectX::XMFLOAT4X4 m_ProjectionMatrix = {};
		DirectX::XMFLOAT4X4 m_ViewMatrix = {};
		DirectX::XMFLOAT4X4 m_ViewProjectionMatrix = {};

		DirectX::XMFLOAT3 m_Position = {};
		float m_Rotation = 0.0f;
	};

}