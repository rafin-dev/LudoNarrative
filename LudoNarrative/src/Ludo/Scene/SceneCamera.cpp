#include "ldpch.h"
#include "SceneCamera.h"

namespace Ludo {

	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	SceneCamera::~SceneCamera()
	{
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_OrthographicsSize = size;
		m_OrthographicsNear = nearClip;
		m_OrthographicFar = farClip;

		RecalculateProjection();
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_AspectRatio = (float)width / (float)height;

		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		float orthoLeft = -m_OrthographicsSize * m_AspectRatio * 0.5f;
		float orthoRight = m_OrthographicsSize * m_AspectRatio * 0.5f;
		float orthoBottom = -m_OrthographicsSize * 0.5f;
		float orthoTop = m_OrthographicsSize * 0.5f;

		DirectX::XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixOrthographicOffCenterLH(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicsNear, m_OrthographicFar));
	}

}