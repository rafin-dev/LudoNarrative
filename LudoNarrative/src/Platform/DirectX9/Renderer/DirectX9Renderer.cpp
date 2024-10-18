#include "ldpch.h"
#include "DirectX9Renderer.h"

#include "Ludo/Log.h"
#include "Ludo/Application.h"
#include "Platform/Windows/WindowsWindow.h"

#include "Platform/DirectX9/imgui/imgui_impl_dx9.h"
#include "Platform/DirectX9/imgui/imgui_impl_win32.h"

namespace Ludo {

	void DirectX9Renderer::Init()
	{
		LD_CORE_INFO("Initializing DirectX9 Render API");

		// Since this is the DirectX9 interface, we can assume the platform is windows
		HWND handle = static_cast<WindowsWindow&>(Application::Get().GetWindow()).GetHandle();

		Interface = Direct3DCreate9(D3D_SDK_VERSION);

		Parameters.Windowed = TRUE;
		Parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		Parameters.hDeviceWindow = handle;

		Interface->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			handle,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&Parameters,
			&Device
		);

		LD_CORE_INFO("Initializing ImGui");
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;
		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(handle);
		ImGui_ImplDX9_Init(Device);
	}

	DirectX9Renderer::~DirectX9Renderer()
	{
		LD_CORE_INFO("Closing ImGui");
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		LD_CORE_INFO("Closing DirectX9 Render API");
		Device->Release();
		Interface->Release();
	}

	void DirectX9Renderer::Resize(unsigned int width, unsigned int height)
	{
		Parameters.BackBufferWidth = width;
		Parameters.BackBufferHeight = height;

		ResetDevice();
	}

	void DirectX9Renderer::BeginScene()
	{
		Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);
		Device->SetRenderState(D3DRS_ZENABLE, FALSE);
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		Device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		Device->BeginScene();

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void DirectX9Renderer::EndScene()
	{
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		Device->EndScene();
		Device->Present(NULL, NULL, NULL, NULL);
	}

	void DirectX9Renderer::ResetDevice()
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		if (Device->Reset(&Parameters) == D3DERR_INVALIDCALL)
		{
			LD_ASSERT(false, "Failed to reset DirectX device");
		}
		ImGui_ImplDX9_CreateDeviceObjects();
	}

	InternalRenderer* InternalRenderer::Get()
	{
		static std::unique_ptr<InternalRenderer> Rend = std::make_unique<DirectX9Renderer>();
		return Rend.get();
	}
}