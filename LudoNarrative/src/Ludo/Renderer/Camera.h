#pragma once

#include <DirectXMath.h>

namespace Ludo {

	class Camera
	{
	public:
		Camera()
		{
			DirectX::XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixIdentity());
		}
		Camera(const DirectX::XMFLOAT4X4& projection)
			: m_Projection(projection) {}
		virtual ~Camera() = default;

		const DirectX::XMFLOAT4X4& GetProjection() const { return m_Projection; }

	protected:
		DirectX::XMFLOAT4X4 m_Projection;

	};

}