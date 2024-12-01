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
			-0.50f, -0.50f, 0.0f,
			-0.50f,  0.50f, 0.0f,
			 0.50f,  0.50f, 0.0f,
			 0.50f, -0.50f, 0.0f
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
			{ "Position", Ludo::ShaderDataType::Float3 }
		};
		Ludo::BufferLayout Material =
		{
			{ "Color", Ludo::ShaderDataType::Float4 }
		};

		m_VertexBuffer.reset(Ludo::VertexBuffer::Create(vertices, sizeof(vertices), Layout));
		m_IndexBuffer.reset(Ludo::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

		auto GetShaderBlob = [](const std::string& name) -> std::pair<void*, size_t>
		{
			void* data = nullptr;
			size_t size = 0;

			wchar_t moduleFileName[MAX_PATH];
			GetModuleFileNameW(nullptr, moduleFileName, MAX_PATH);

			std::filesystem::path shaderPath = moduleFileName;
			shaderPath.remove_filename();
			shaderPath += "../LudoNarrative";
			shaderPath = shaderPath / name;
			std::ifstream shaderIn(shaderPath, std::ios::binary);

			if (shaderIn.is_open())
			{
				size = std::filesystem::file_size(shaderPath);
				data = malloc(size);
				if (data != nullptr)
				{
					shaderIn.read((char*)data, size);
				}
			}

			return std::pair<void*, size_t>(data, size);
		};

		auto vertexShader = GetShaderBlob("VertexShader.cso");
		auto pixelShader = GetShaderBlob("PixelShader.cso");

		Ludo::LUDO_SHADER_DESC desc;
		desc.VertexShaderBlob = vertexShader.first;
		desc.VertexShaderSize = vertexShader.second;
		desc.PixelShaderBlob = pixelShader.first;
		desc.PixelShaderSize = pixelShader.second;
		desc.VertexBufferLayout = m_VertexBuffer->GetLayout();
		desc.MaterialDataLayout = Material;
		
		m_Shader.reset(Ludo::Shader::Create(desc));
		m_Material = Ludo::Material::Create(m_Shader);
		float color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		m_Material->SetMaterialItemData("Color", color);

		free(vertexShader.first);
		free(pixelShader.first);
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
	std::shared_ptr<Ludo::Shader> m_Shader;

	std::shared_ptr<Ludo::Material> m_Material;

	std::shared_ptr<Ludo::VertexBuffer> m_VertexBuffer;
	std::shared_ptr<Ludo::IndexBuffer> m_IndexBuffer;

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