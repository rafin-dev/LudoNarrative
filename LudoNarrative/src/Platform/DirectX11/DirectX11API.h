#pragma once

#include "ldpch.h"
#include "Ludo/Renderer/RendererAPI.h"
#include "Ludo/Log.h"
#include "imgui/imgui.h"

namespace Ludo {

	class DirectX11API : public RendererAPI
	{
	public:
		virtual bool Init() override;
		virtual void ShutDown() override;

		virtual void SetClearColor(const DirectX::XMFLOAT4& color) override;
		virtual const DirectX::XMFLOAT4& GetClearColor() override;

		virtual void DrawIndexed(const std::shared_ptr<VertexBuffer>& vertexBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer) override;

		virtual void BeginImGui() override;
		virtual void EndImGui() override;

		static DirectX11API* Get();

		auto*& const GetFactory() { return m_Factory; }
		auto*& const GetDevice() { return m_Device; }
		auto*& const GetDeviceContext() { return m_DeviceContext; }

	private:
		ID3D11Device* m_Device = nullptr;
		ID3D11DeviceContext* m_DeviceContext = nullptr;
		IDXGIFactory2* m_Factory = nullptr;

		DirectX::XMFLOAT4 m_ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	};

}