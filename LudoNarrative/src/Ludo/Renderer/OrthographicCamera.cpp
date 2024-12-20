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

		DirectX::XMVECTOR Eye = DirectX::XMVectorSet(m_Position.x, m_Position.y, -1.0f, 0.0f);
		DirectX::XMVECTOR LookAt = DirectX::XMVectorSet(m_Position.x, m_Position.y, 0.0f, 0.0f);
		DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, LookAt, Up);

		DirectX::XMStoreFloat4x4(&m_ViewProjectionMatrix, DirectX::XMMatrixTranspose(View * DirectX::XMLoadFloat4x4(&m_ProjectionMatrix)));
		DirectX::XMStoreFloat4x4(&m_ViewMatrix, View);
	}

}