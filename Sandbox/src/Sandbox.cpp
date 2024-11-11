#include <LudoEngine.h>

#include <iostream>

#include "imgui/imgui.h"

#include "Ludo/Renderer/Buffer.h"
#include "Ludo/Renderer/Shader.h"

#include "Ludo/Renderer/Renderer.h"
#include "Ludo/Renderer/OrthographicCamera.h"

class ExampleLayer : public Ludo::Layer
{
public:
	ExampleLayer()
	: Ludo::Layer("Example"), Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		float vertices[] =
		{
			-0.75f, -0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f
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

		VertexBuffer.reset(Ludo::VertexBuffer::Create(vertices, sizeof(vertices), Layout));
		IndexBuffer.reset(Ludo::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

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
		desc.TargetPipeline = Ludo::LUDO_TARGET_PIPELINE_2D;
		desc.VertexShaderBlob = vertexShader.first;
		desc.VertexShaderSize = vertexShader.second;
		desc.PixelShaderBlob = pixelShader.first;
		desc.PixelShaderSize = pixelShader.second;
		desc.Layout = VertexBuffer->GetLayout();
		
		Shader.reset(Ludo::Shader::Create(desc));
		free(vertexShader.first);
		free(pixelShader.first);
	}

	void OnUpdate() override
	{
		auto pos = Camera.GetPosition();

		pos.x += (Ludo::Input::IsKeyPressed(LD_KEY_A) - Ludo::Input::IsKeyPressed(LD_KEY_D)) * 0.1f;
		pos.y += (Ludo::Input::IsKeyPressed(LD_KEY_S) - Ludo::Input::IsKeyPressed(LD_KEY_W)) * 0.1f;

		Camera.SetPosition(pos);

		Camera.SetRotation(Camera.GetRotation() + Ludo::Input::IsMouseButtonDown(LD_MOUSE_BUTTON_LEFT) * 0.1f);

		Ludo::Renderer::BeginScene(Camera);

		Ludo::Renderer::Submit(Shader, VertexBuffer, IndexBuffer);

		Ludo::Renderer::EndScene();
	}

	void OnEvent(Ludo::Event& event) override
	{
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Color");

		static float color[4];
		ImGui::ColorPicker4("Clear Color", color);

		Ludo::RenderCommand::SetClearColor(DirectX::XMFLOAT4(color));

		ImGui::End();
	}

	std::shared_ptr<Ludo::Shader> Shader;

	std::shared_ptr<Ludo::VertexBuffer> VertexBuffer;
	std::shared_ptr<Ludo::IndexBuffer> IndexBuffer;

	Ludo::OrthographicCamera Camera;
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