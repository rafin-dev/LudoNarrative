#include "ldpch.h"
#include "Transform.h"

namespace Ludo {

	Transform::Transform(float x, float y, float z, float r, float s)
		: Position({ x, y, z }), Rotation(r), Scale({ s, s, s }), m_ModelMatrix(DirectX::XMFLOAT4X4())
	{
	}

	DirectX::XMFLOAT4X4 Transform::GetModelMarix()
	{
		DirectX::XMStoreFloat4x4(&m_ModelMatrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z) * 
			DirectX::XMMatrixRotationZ(Rotation) * 
			DirectX::XMMatrixTranslation(Position.x, Position.y, Position.z)));

		return m_ModelMatrix;
	}

}