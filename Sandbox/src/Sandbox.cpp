#include <LudoNarrative.h>
#include <Ludo/Core/EntryPoint.h>

#include "imgui/imgui.h"

#include "Sandbox2D.h"

class ExampleLayer : public Ludo::Layer
{
public:
	ExampleLayer()
	: Ludo::Layer("Example"), m_CameraController(1280.0f / 720.0f, true)
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

		auto vertexBuffer = Ludo::VertexBuffer::Create(vertices, sizeof(vertices), Layout, Ludo::VertexBuffer::IMMUTABLE);
		auto indexBuffer = Ludo::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		m_VertexArray = Ludo::VertexArray::Create();
		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(indexBuffer);
		
		m_Shader = m_ShaderLibrary.Load("TextureShader", "assets/shaders/TextureShader.hlsl", vertexBuffer->GetLayout(), Material);
		m_Material = Ludo::Material::Create(m_Shader);
		float color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		m_Material->SetMaterialItemData("Color", color);

		m_BoardTexture = Ludo::Texture2D::Create("assets/textures/Checkerboard.png");
		m_ChernoTexture = Ludo::Texture2D::Create("assets/textures/ChernoLogo.png");

		m_FPSs.resize(10);
	}

	~ExampleLayer()
	{
	}

	void OnUpdate(Ludo::TimeStep time) override
	{
		m_FPSs[m_cFPS] = 1 / time;
		m_cFPS = (m_cFPS + 1) % 10;
		
		m_Transform.Position.x += (Ludo::Input::IsKeyPressed(LD_KEY_RIGHT_ARROW) - Ludo::Input::IsKeyPressed(LD_KEY_LEFT_ARROW)) * 2.0f * time;
		m_Transform.Position.y += (Ludo::Input::IsKeyPressed(LD_KEY_UP_ARROW) - Ludo::Input::IsKeyPressed(LD_KEY_DOWN_ARROW)) * 2.0f * time;

		m_CameraController.OnUpdate(time);

		Ludo::Renderer::BeginScene(m_CameraController.GetCamera());

		m_Material->UploadMaterialData();

		m_BoardTexture->Bind();
		Ludo::Renderer::Submit(m_Material, m_VertexArray, m_Transform.GetModelMarix());

		m_ChernoTexture->Bind();
		m_VertexArray->Bind();
		Ludo::Renderer::Submit(m_Material, m_VertexArray, m_Transform.GetModelMarix());

		Ludo::Renderer::EndScene();
	}

	void OnEvent(Ludo::Event& event) override
	{
		m_CameraController.OnEvent(event);
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");

		std::stringstream ss;
		ss << "FPS: " << std::roundf(std::accumulate(m_FPSs.begin(), m_FPSs.end(), 0) / 10.0f);
		ImGui::Text(ss.str().c_str());

		static float color[4];
		ImGui::ColorEdit4("Clear Color", color);
		Ludo::RenderCommand::SetClearColor(DirectX::XMFLOAT4(color));

		ImGui::End();
	}

private:
	Ludo::ShaderLibrary m_ShaderLibrary;
	Ludo::Ref<Ludo::Shader> m_Shader;

	Ludo::Ref<Ludo::Material> m_Material;
	Ludo::Ref<Ludo::Texture2D> m_BoardTexture;
	Ludo::Ref<Ludo::Texture2D> m_ChernoTexture;

	Ludo::Ref<Ludo::VertexArray> m_VertexArray;

	Ludo::OrthographicCameraController m_CameraController;

	Ludo::Transform m_Transform;
	std::vector<float> m_FPSs;
	size_t m_cFPS = 0;
};

class Sandbox : public Ludo::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{

	}
};

Ludo::Application* Ludo::CreateApplication()
{
	return new Sandbox();
}