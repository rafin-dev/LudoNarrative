#pragma once

#include "Ludo/Renderer/RendererAPI.h"
#include "Ludo/Core/Log.h"
#include <imgui.h>

namespace Ludo {

	class DirectX11API : public RendererAPI
	{
	public:
		bool Init() override;
		void ShutDown() override;

		void SetClearColor(const DirectX::XMFLOAT4& color) override;
		const DirectX::XMFLOAT4& GetClearColor() override;

		void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) override;
		void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;

		void BeginImGui() override;
		void EndImGui() override;

		static DirectX11API* Get();

		auto*& const GetFactory() { return m_Factory; }
		auto*& const GetDevice() { return m_Device; }
		auto*& const GetDeviceContext() { return m_DeviceContext; }

	private:
		ID3D11Device* m_Device = nullptr;
		ID3D11DeviceContext* m_DeviceContext = nullptr;
		IDXGIFactory7* m_Factory = nullptr;

		ID3D11RasterizerState* m_RasterizerState = nullptr;

		DirectX::XMFLOAT4 m_ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	};

}