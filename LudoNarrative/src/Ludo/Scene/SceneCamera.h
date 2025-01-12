#pragma once

#include "Ludo/Renderer/Camera.h"

namespace Ludo {

	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		~SceneCamera() override;

		void SetOrthographic(float size, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		float GetOrthographicsSize() const { return m_OrthographicsSize; }
		void SetOrthographicSize(float size) { m_OrthographicsSize = size; RecalculateProjection(); }

	private:
		void RecalculateProjection();

		float m_OrthographicsSize = 10.0f;
		float m_OrthographicsNear = 0.0f;
		float m_OrthographicFar = 10.0f;

		float m_AspectRatio = 1.0f;
	};

}