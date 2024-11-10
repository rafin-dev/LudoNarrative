#include <LudoEngine.h>

#include <iostream>

#include "imgui/imgui.h"

#include "Ludo/Renderer/Buffer.h"
#include "Ludo/Renderer/Shader.h"

#include "Platform/DirectX12/DirectX12System.h"

class ExampleLayer : public Ludo::Layer
{
public:
	ExampleLayer()
		: Ludo::Layer("Example")
	{
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

		Shader.reset(Ludo::Shader::Create(desc));
		free(vertexShader.first);
		free(pixelShader.first);

		float vertices[] =
		{
			-0.5f, -0.5f,
			-0.5f,  0.5f,
			 0.5f,  0.5f,
			 0.5f, -0.5f
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

		VertexBuffer.reset(Ludo::VertexBuffer::Create(vertices, sizeof(vertices)));
		IndexBuffer.reset(Ludo::IndexBuffer::Create(indices, sizeof(indices) / sizeof(indices[0])));
	}

	void OnUpdate() override
	{
		Shader->Bind();
		VertexBuffer->Bind();
		IndexBuffer->Bind();

		Ludo::DirectX12System::Get()->GetCommandList()->DrawIndexedInstanced(IndexBuffer->GetCount(), 1, 0, 0, 0);
	}

	void OnEvent(Ludo::Event& event) override
	{
	}

	void OnImGuiRender() override
	{
		//ImGui::ShowDemoWindow();
	}

	std::unique_ptr<Ludo::Shader> Shader;

	std::unique_ptr<Ludo::VertexBuffer> VertexBuffer;
	std::unique_ptr<Ludo::IndexBuffer> IndexBuffer;
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