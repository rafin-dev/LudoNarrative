#include "ldpch.h"
#include "DirectX11Shader.h"

#include "DirectX11API.h"
#include "Utils/DirectX11Utils.h"

namespace Ludo {

	DirectX11Shader::DirectX11Shader(
		const std::string& name,
		void* vertexShaderBuffer, size_t vertexShaderSize,
		void* pixelShaderBuffer, size_t pixelShaderSize,
		const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout)
		: m_Name(name), m_VertexBufferLayout(vertexLayout), m_MaterialDataLayout(materialDataLayout)
	{
		LD_PROFILE_FUNCTION();

		std::unordered_map<ShaderKind, ID3DBlob*> shaders
		{
			{ VertexShader, nullptr },
			{ PixelShader, nullptr }
		};

		HRESULT hr = D3DCreateBlob(vertexShaderSize, &shaders[VertexShader]);
		LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to create D3DBBlob");
		memcpy(shaders[VertexShader]->GetBufferPointer(), vertexShaderBuffer, vertexShaderSize);

		hr = D3DCreateBlob(pixelShaderSize, &shaders[PixelShader]);
		LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to create Shader Blob");
		memcpy(shaders[PixelShader]->GetBufferPointer(), pixelShaderBuffer, pixelShaderSize);

		Init(shaders);

		for (auto& blob : shaders)
		{
			CHECK_AND_RELEASE_COMPTR(blob.second);
		}
	}

	DirectX11Shader::DirectX11Shader(const std::string& name, const std::filesystem::path& shaderSrcPath,
		const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout)
		: m_Name(name), m_VertexBufferLayout(vertexLayout), m_MaterialDataLayout(materialDataLayout)
	{
		LD_PROFILE_FUNCTION();

		std::unordered_map<ShaderKind, std::string> shadersSources;
		std::unordered_map<ShaderKind, ID3D10Blob*> shaderBinaries;
		std::string source;

		ReadFile(shaderSrcPath, source);
		ParseShaders(source, shadersSources);
		CompileShaders(shadersSources, shaderBinaries);
		Init(shaderBinaries);
		for (auto& shaderBlob : shaderBinaries)
		{
			CHECK_AND_RELEASE_COMPTR(shaderBlob.second);
		}
	}

	DirectX11Shader::ShaderKind ShaderKindFromString(const std::string& kind)
	{
		if (kind == "vertex")
		{
			return DirectX11Shader::ShaderKind::VertexShader;
		}
		else if (kind == "pixel")
		{
			return DirectX11Shader::ShaderKind::PixelShader;
		}

		LD_CORE_ASSERT(false, "Syntax Error: Unknown Shader Kind '{0}'", kind);
	}

	void DirectX11Shader::ReadFile(const std::filesystem::path& file, std::string& output)
	{
		LD_PROFILE_FUNCTION();

		std::ifstream srcFile(file, std::ios::in | std::ios::binary);
		if (srcFile)
		{
			size_t size = std::filesystem::file_size(file);
			output.resize(size);
			srcFile.seekg(0, std::ios::beg);
			srcFile.read(&output[0], size);
		}
		else
		{
			LD_CORE_ASSERT(false, "Failed to open file: {0}", file.string());
		}

	}

	void DirectX11Shader::ParseShaders(const std::string& source, std::unordered_map<ShaderKind, std::string>& shadersSrcs)
	{
		LD_PROFILE_FUNCTION();

		const char* typeToken = "#kind";
		size_t typeTokenLenght = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			LD_CORE_ASSERT(eol != std::string::npos, "Syntax Error: Shader declared but not defined!");
			size_t begin = pos + typeTokenLenght + 1; // Move to "#type "<-
			std::string kind = source.substr(begin, eol - begin);

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			LD_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax Error: Shader declared but not defined!");

			pos = source.find(typeToken, nextLinePos);

			shadersSrcs[ShaderKindFromString(kind)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}
	}

	void DirectX11Shader::CompileShaders(const std::unordered_map<ShaderKind, std::string> shadersSources, std::unordered_map<ShaderKind, ID3D10Blob*>& output)
	{
		LD_PROFILE_FUNCTION();

		UINT flags = NULL;
#ifdef LUDO_DEBUG
		flags |= D3DCOMPILE_DEBUG;
#endif

		for (auto& [shaderKind, shaderSrc] : shadersSources)
		{
			const char* target;
			switch (shaderKind)
			{
			case VertexShader: target = "vs_5_0"; break;
			case PixelShader: target = "ps_5_0"; break;
			default: LD_CORE_ASSERT(false, "Syntax Error: Unknown Shader Kind");
			}

			ID3DBlob* errorMessage = nullptr;

			output[shaderKind] = nullptr;
			HRESULT hr = D3DCompile(
				shaderSrc.c_str(),
				shaderSrc.size(),
				nullptr,
				nullptr,
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				"main",
				target,
				flags,
				NULL,
				&output[shaderKind],
				&errorMessage
			);
			LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to compile Shader: {0}", (char*)errorMessage->GetBufferPointer());
		}
	}

	static DXGI_FORMAT GetDxgiFormatFromShaderDataType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:   return DXGI_FORMAT_R32_FLOAT;
		case ShaderDataType::Float2:  return DXGI_FORMAT_R32G32_FLOAT;
		case ShaderDataType::Float3:  return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::Float4:  return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ShaderDataType::Int:     return DXGI_FORMAT_R32_SINT;
		case ShaderDataType::Int2:    return DXGI_FORMAT_R32G32_SINT;
		case ShaderDataType::Int3:    return DXGI_FORMAT_R32G32B32_SINT;
		case ShaderDataType::Int4:    return DXGI_FORMAT_R32G32B32A32_SINT;
		case ShaderDataType::Uint:    return DXGI_FORMAT_R32_UINT;
		case ShaderDataType::Uint2:   return DXGI_FORMAT_R32G32_UINT;
		case ShaderDataType::Uint3:   return DXGI_FORMAT_R32G32B32_UINT;
		case ShaderDataType::Uint4:   return DXGI_FORMAT_R32G32B32A32_UINT;
		case ShaderDataType::Bool:    return DXGI_FORMAT_R8_TYPELESS;
		}
	}

	bool DirectX11Shader::Init(std::unordered_map<ShaderKind, ID3DBlob*> shaders)
	{
		LD_PROFILE_FUNCTION();

		HRESULT hr = S_OK;
		auto device = DirectX11API::Get()->GetDevice();

		// ========== Shaders ==========
		hr = device->CreateVertexShader(shaders[VertexShader]->GetBufferPointer(), shaders[VertexShader]->GetBufferSize(), NULL, &m_VertexShader);
		VALIDATE_DX_HRESULT(hr, "Failed to create Vertex Shader");
		hr = device->CreatePixelShader(shaders[PixelShader]->GetBufferPointer(), shaders[PixelShader]->GetBufferSize(), NULL, &m_PixelShader);
		VALIDATE_DX_HRESULT(hr, "Failed to create Pixel Shader");

		// ========== Vertex Buffer Layout & Material Data Layout ==========
		std::vector<D3D11_INPUT_ELEMENT_DESC> ElementLayout;
		ElementLayout.reserve(m_VertexBufferLayout.GetElements().size());
		for (auto& element : m_VertexBufferLayout)
		{
			D3D11_INPUT_ELEMENT_DESC desc;
			if (element.Type == ShaderDataType::Float3x3)
			{
				for (uint32_t i = 0; i < 3; i++)
				{
					desc =
					{ element.Name.c_str(), i, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
					ElementLayout.push_back(desc);
				}
			}
			else if (element.Type == ShaderDataType::Float4x4)
			{
				for (uint32_t i = 0; i < 4; i++)
				{
					desc =
					{ element.Name.c_str(), i, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
					ElementLayout.push_back(desc);
				}
			}

			desc =
			{ element.Name.c_str(), 0, GetDxgiFormatFromShaderDataType(element.Type), 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			ElementLayout.push_back(desc);
		}
		hr = device->CreateInputLayout(ElementLayout.data(), ElementLayout.size(), shaders[VertexShader]->GetBufferPointer(), shaders[VertexShader]->GetBufferSize(), &m_InputLayout);
		VALIDATE_DX_HRESULT(hr, "Failed to create Input Layout");

		LD_CORE_TRACE("Created Shader '{0}'", m_Name);

		return true;
	}

	DirectX11Shader::~DirectX11Shader()
	{
		ShutDown();
	}

	void DirectX11Shader::Bind()
	{
		LD_PROFILE_FUNCTION();

		auto deviceContext = DirectX11API::Get()->GetDeviceContext();

		deviceContext->VSSetShader(m_VertexShader, 0, 0);
		deviceContext->PSSetShader(m_PixelShader, 0, 0);
		deviceContext->IASetInputLayout(m_InputLayout);
	}

	const BufferLayout& DirectX11Shader::GetVertexBufferLayout()
	{
		return m_VertexBufferLayout;
	}

	const BufferLayout& DirectX11Shader::GetMaterialLayout()
	{
		return m_MaterialDataLayout;
	}

	void DirectX11Shader::ShutDown()
	{
		LD_PROFILE_FUNCTION();

		CHECK_AND_RELEASE_COMPTR(m_VertexShader);
		CHECK_AND_RELEASE_COMPTR(m_PixelShader);
		CHECK_AND_RELEASE_COMPTR(m_InputLayout);

		LD_CORE_TRACE("Deleted Shader: {0}", m_Name);
	}

}