#include "ldpch.h"
#include "DirectX11Renderer.h"

#include "Ludo/Log.h"
#include "Ludo/Application.h"
#include "Platform/Windows/WindowsWindow.h"

#include "imgui.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include "imgui/backends/imgui_impl_win32.h"

#include "DirectX11Utils.h"

namespace Ludo {

	bool DirectX11Renderer::Init()
	{
		HRESULT hr = S_OK;

		LD_CORE_INFO("Initializing DirectX11");
		hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			NULL,
			NULL,
			NULL,
			D3D11_SDK_VERSION,
			&m_Device,
			NULL,
			&m_DeviceContext
		);
		if (FAILED(hr))
		{
			LD_CORE_ERROR("Failed to create DirectX11 Device");
			ShutDown();
			return false;
		}

		LD_CORE_INFO("Initializing ImGui");
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;

		ImGui_ImplDX11_Init(m_Device, m_DeviceContext);

		return true;
	}

	DirectX11Renderer::~DirectX11Renderer()
	{
		ShutDown();
	}

	void DirectX11Renderer::BeginImGui()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void DirectX11Renderer::EndImGui()
	{
		ImGui::Render();

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void DirectX11Renderer::ShutDown()
	{
		LD_CORE_INFO("CLosing DirectX11");
		CHECK_AND_RELEASE(m_Device);
		CHECK_AND_RELEASE(m_DeviceContext);
	}

	InternalRenderer* InternalRenderer::Get()
	{
		static DirectX11Renderer rend;
		return &rend;
	}

}