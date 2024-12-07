#include "ldpch.h"
#include "OrthographicCamera.h"

namespace Ludo {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
	{
		SetProjection(left, right, bottom, top);

		DirectX::XMStoreFloat4x4(&m_ViewMatrix, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&m_ViewProjectionMatrix, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_ViewMatrix) * DirectX::XMLoadFloat4x4(&m_ProjectionMatrix)));
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		DirectX::XMStoreFloat4x4(&m_ProjectionMatrix, DirectX::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, -1, 1));
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		DirectX::XMMATRIX tranformation = DirectX::XMMatrixRotationZ(m_Rotation) * DirectX::XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
		DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixInverse(nullptr, tranformation);

		DirectX::XMStoreFloat4x4(&m_ViewProjectionMatrix, DirectX::XMMatrixTranspose(viewMatrix * DirectX::XMLoadFloat4x4(&m_ProjectionMatrix)));
		DirectX::XMStoreFloat4x4(&m_ViewMatrix, DirectX::XMMatrixTranspose(viewMatrix));
	}

}