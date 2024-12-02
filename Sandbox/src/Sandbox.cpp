#include <LudoEngine.h>

#include <iostream>

#include "imgui/imgui.h"

class ExampleLayer : public Ludo::Layer
{
public:
	ExampleLayer()
	: Ludo::Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		float vertices[] =
		{
			-0.50f, -0.50f, 0.0f, 0.0f, 0.0f,
			-0.50f,  0.50f, 0.0f, 0.0f, 1.0f,
			 0.50f,  0.50f, 0.0f, 1.0f, 1.0f,
			 0.50f, -0.50f, 0.0f, 1.0f, 0.0f
		};
		uint32_t indices[] =
		{
			0,
			1,
			2,
			2,
			3,
			0
		};

		Ludo::BufferLayout Layout =
		{
			{ "Position", Ludo::ShaderDataType::Float3 },
			{ "TexCoord", Ludo::ShaderDataType::Float2 }
		};
		Ludo::BufferLayout Material =
		{
			{ "Color", Ludo::ShaderDataType::Float4 }
		};

		m_VertexBuffer = Ludo::VertexBuffer::Create(vertices, sizeof(vertices), Layout);
		m_IndexBuffer = Ludo::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		
		m_Shader = m_ShaderLibrary.Load("TextureShader", "assets/shaders/VertexShader.hlsl", "assets/shaders/PixelShader.hlsl", m_VertexBuffer->GetLayout(), Material);
		m_Material = Ludo::Material::Create(m_Shader);
		float color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		m_Material->SetMaterialItemData("Color", color);

		m_BoardTexture = Ludo::Texture2D::Create("assets/textures/Checkerboard.png");
		m_ChernoTexture = Ludo::Texture2D::Create("assets/textures/ChernoLogo.png");
	}

	~ExampleLayer()
	{
	}

	void OnUpdate(Ludo::TimeStep time) override
	{
		auto pos = m_Camera.GetPosition();

		pos.x += (Ludo::Input::IsKeyPressed(LD_KEY_D) - Ludo::Input::IsKeyPressed(LD_KEY_A)) * 2.0f * time;
		pos.y += (Ludo::Input::IsKeyPressed(LD_KEY_W) - Ludo::Input::IsKeyPressed(LD_KEY_S)) * 2.0f * time;

		m_Camera.SetPosition(pos);

		m_Camera.SetRotation(m_Camera.GetRotation() + (Ludo::Input::IsKeyPressed(LD_KEY_E) - Ludo::Input::IsKeyPressed(LD_KEY_Q)) * 2.0f * time);

		m_Transform.Position.x += (Ludo::Input::IsKeyPressed(LD_KEY_RIGHT_ARROW) - Ludo::Input::IsKeyPressed(LD_KEY_LEFT_ARROW)) * 2.0f * time;
		m_Transform.Position.y += (Ludo::Input::IsKeyPressed(LD_KEY_UP_ARROW) - Ludo::Input::IsKeyPressed(LD_KEY_DOWN_ARROW)) * 2.0f * time;

		Ludo::Renderer::BeginScene(m_Camera);

		m_Material->UploadMaterialData();

		m_BoardTexture->Bind();
		Ludo::Renderer::Submit(m_Shader, m_VertexBuffer, m_IndexBuffer, m_Transform.GetModelMarix());

		m_ChernoTexture->Bind();
		Ludo::Renderer::Submit(m_Shader, m_VertexBuffer, m_IndexBuffer, m_Transform.GetModelMarix());

		Ludo::Renderer::EndScene();
	}

	void OnEvent(Ludo::Event& event) override
	{
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");

		static float color[4];
		ImGui::ColorEdit4("Clear Color", color);
		Ludo::RenderCommand::SetClearColor(DirectX::XMFLOAT4(color));

		static float sqColor[4];
		ImGui::ColorEdit4("Square Color", sqColor);
		m_Material->SetMaterialItemData("Color", sqColor);

		ImGui::End();
	}

private:
	Ludo::ShaderLibrary m_ShaderLibrary;
	Ludo::Ref<Ludo::Shader> m_Shader;

	Ludo::Ref<Ludo::Material> m_Material;
	Ludo::Ref<Ludo::Texture2D> m_BoardTexture;
	Ludo::Ref<Ludo::Texture2D> m_ChernoTexture;

	Ludo::Ref<Ludo::VertexBuffer> m_VertexBuffer;
	Ludo::Ref<Ludo::IndexBuffer> m_IndexBuffer;

	Ludo::OrthographicCamera m_Camera;

	Ludo::Transform m_Transform;
};

class Sandbox : public Ludo::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}
};

Ludo::Application* Ludo::CreateApplication()
{
	return new Sandbox();
}