#pragma once

#include "Ludo/Renderer/InternalRenderer.h"

namespace Ludo {

	class DirectX11Renderer : public InternalRenderer
	{
	public:
		bool Init() override;
		~DirectX11Renderer() override;

		void BeginImGui() override;
		void EndImGui() override;

		inline ID3D11Device* GetDevice() { return m_Device; }
		inline ID3D11DeviceContext* GetDeviceContext() { return m_DeviceContext; }

		static inline DirectX11Renderer* Get() { return (DirectX11Renderer*)InternalRenderer::Get(); }

	private:
		ID3D11Device* m_Device = nullptr;
		ID3D11DeviceContext* m_DeviceContext = nullptr;

		void ShutDown();
	};

}