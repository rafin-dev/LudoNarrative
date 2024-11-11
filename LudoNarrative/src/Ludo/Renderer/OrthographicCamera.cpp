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

		// I need to deeply study Matrix multiplications because I do not know if or why this is correct
		DirectX::XMMATRIX tranformation = DirectX::XMMatrixTranslationFromVector(pos) * DirectX::XMMatrixRotationZ(m_Rotation);
		DirectX::XMStoreFloat4x4(&m_ViewMatrix, DirectX::XMMatrixInverse(nullptr, tranformation));

		DirectX::XMStoreFloat4x4(&m_ViewProjectionMatrix, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_ViewMatrix) * DirectX::XMLoadFloat4x4(&m_ProjectionMatrix)));
		// The code look kinad weird because of DirectXMath memory loaction requirements
		// So I cant really do m_ViewMatrix = [cool calculation]
		// :(
	}

}