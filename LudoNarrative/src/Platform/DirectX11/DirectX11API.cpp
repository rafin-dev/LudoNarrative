#include "ldpch.h"
#include "DirectX11API.h"

#include "Utils/DirectX11Utils.h"

#include "imgui.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

namespace Ludo {

    bool DirectX11API::Init()
    {
        LD_PROFILE_FUNCTION();

        HRESULT hr = S_OK;

        hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_Factory));
        VALIDATE_DX_HRESULT(hr, "Failed to create DXGI Factory");

        UINT flag = NULL;
#ifdef LUDO_DEBUG
        flag = D3D11_CREATE_DEVICE_DEBUG;
#endif // LUDO_DEBUG

        hr = D3D11CreateDevice(
            NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            flag,
            NULL,
            NULL,
            D3D11_SDK_VERSION,
            &m_Device,
            NULL,
            &m_DeviceContext);
        VALIDATE_DX_HRESULT(hr, "Failed to create D3D11 Device/Device Context");

        D3D11_RASTERIZER_DESC rastesDesc = {};
        rastesDesc.FillMode = D3D11_FILL_SOLID;
        rastesDesc.CullMode = D3D11_CULL_NONE;
        rastesDesc.FrontCounterClockwise = false;
        rastesDesc.DepthBias = 0;
        rastesDesc.SlopeScaledDepthBias = 0.0f;
        rastesDesc.DepthBiasClamp = 0.0f;
        rastesDesc.DepthClipEnable = false;
        rastesDesc.ScissorEnable = false;
        rastesDesc.MultisampleEnable = false;
        rastesDesc.AntialiasedLineEnable = false;

        hr = m_Device->CreateRasterizerState(&rastesDesc, &m_RasterizerState);
        VALIDATE_DX_HRESULT(hr, "Failed to create Rasterizer state");

        m_DeviceContext->RSSetState(m_RasterizerState);

        LD_CORE_INFO("Initalized DirectX11 Render API");

        // ImGui
        {
            LD_PROFILE_SCOPE("Init ImGui - DirectX11API");

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

            ImGui::StyleColorsDark();
            ImGuiStyle& style = ImGui::GetStyle();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }

            ImGui_ImplDX11_Init(m_Device, m_DeviceContext);

            LD_CORE_INFO("Initialized ImGui");
        }

        return true;
    }

    void DirectX11API::ShutDown()
    {
        LD_PROFILE_FUNCTION();

        {
            LD_PROFILE_SCOPE("Close ImGui - DirectX11API::ShutDown");
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
            LD_CORE_INFO("Closed ImGui");
        }

        CHECK_AND_RELEASE_COMPTR(m_RasterizerState);

        CHECK_AND_RELEASE_COMPTR(m_Device);
        CHECK_AND_RELEASE_COMPTR(m_DeviceContext);
        CHECK_AND_RELEASE_COMPTR(m_Factory);
        LD_CORE_INFO("Closed DirectX11 Render API");
    }

    void DirectX11API::SetClearColor(const DirectX::XMFLOAT4& color)
    {
        m_ClearColor = color;
    }

    const DirectX::XMFLOAT4& DirectX11API::GetClearColor()
    {
        return m_ClearColor;
    }

    void DirectX11API::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
    {
        LD_PROFILE_RENDERER_FUNCTION();

        vertexArray->Bind();
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_DeviceContext->DrawIndexed(indexCount == 0 ? vertexArray->GetIndexBuffer()->GetCount() : indexCount, 0, 0);
    }

    void DirectX11API::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
    {
        LD_PROFILE_RENDERER_FUNCTION();

        vertexArray->Bind();
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
        m_DeviceContext->DrawInstanced(vertexCount, 1, 0, 0);
    }

    void DirectX11API::BeginImGui()
    {
        LD_PROFILE_RENDERER_FUNCTION();

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void DirectX11API::EndImGui()
    {
        LD_PROFILE_RENDERER_FUNCTION();

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    DirectX11API* DirectX11API::Get()
    {
        static DirectX11API instance;
        return &instance;
    }

}