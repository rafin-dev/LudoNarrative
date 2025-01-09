#include "ldpch.h"
#include "OrthographicCamera.h"

namespace Ludo {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
	{
		LD_PROFILE_FUNCTION();

		SetProjection(left, right, bottom, top);

		DirectX::XMStoreFloat4x4(&m_ViewMatrix, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&m_ViewProjectionMatrix, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_ViewMatrix) * DirectX::XMLoadFloat4x4(&m_ProjectionMatrix)));
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		LD_PROFILE_FUNCTION();

		DirectX::XMStoreFloat4x4(&m_ProjectionMatrix, DirectX::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, 10.0f, 0.0f));
		DirectX::XMStoreFloat4x4(&m_ViewProjectionMatrix, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_ViewMatrix) * DirectX::XMLoadFloat4x4(&m_ProjectionMatrix)));
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		LD_PROFILE_FUNCTION();

		DirectX::XMMATRIX View = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationZ(m_Rotation) *
		DirectX::XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z));

		DirectX::XMStoreFloat4x4(&m_ViewProjectionMatrix, DirectX::XMMatrixTranspose(View * DirectX::XMLoadFloat4x4(&m_ProjectionMatrix)));
		DirectX::XMStoreFloat4x4(&m_ViewMatrix, View);
	}

}