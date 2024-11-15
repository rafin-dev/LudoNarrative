#pragma once

#include "ldpch.h"

namespace Ludo {

	class Transform
	{
	public:
		Transform() = default;
		Transform(float x, float y, float z, float r, float s);

		DirectX::XMFLOAT4X4 GetModelMarix();

		DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };
		float Rotation = 0.0f;
		DirectX::XMFLOAT3 Scale = { 1.0f, 1.0f, 1.0f };

	private:
		DirectX::XMFLOAT4X4 m_ModelMatrix;
	};

}