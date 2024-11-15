#include "ldpch.h"
#include "OrthographicCamera.h"

namespace Ludo {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
	{
		DirectX::XMStoreFloat4x4(&m_ProjectionMatrix, DirectX::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, -1, 1));

		DirectX::XMStoreFloat4x4(&m_ViewMatrix, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&m_ViewProjectionMatrix, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_ViewMatrix) * DirectX::XMLoadFloat4x4(&m_ProjectionMatrix)));
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&m_Position);

		DirectX::XMMATRIX tranformation = DirectX::XMMatrixRotationZ(m_Rotation) * DirectX::XMMatrixTranslationFromVector(pos);
		DirectX::XMStoreFloat4x4(&m_ViewMatrix, DirectX::XMMatrixInverse(nullptr, tranformation));

		DirectX::XMStoreFloat4x4(&m_ViewProjectionMatrix, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_ViewMatrix) * DirectX::XMLoadFloat4x4(&m_ProjectionMatrix)));
	}

}