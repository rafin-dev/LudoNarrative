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

		// ========== View Projection / Model Matrix Buffer ==========
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		hr = device->CreateBuffer(&bufferDesc, nullptr, &m_ViewProjectionBuffer);
		VALIDATE_DX_HRESULT(hr, "Failed to create Constant Buffer for the View Projection Matrix");

		hr = device->CreateBuffer(&bufferDesc, nullptr, &m_ModelMatrixBuffer);
		VALIDATE_DX_HRESULT(hr, "Failed to create Constant Buffer for the Model Matrix");

		bufferDesc.ByteWidth = sizeof(float) * 4;
		hr = device->CreateBuffer(&bufferDesc, nullptr, &m_MaterialBuffer);
		VALIDATE_DX_HRESULT(hr, "Failed to create Material Constant Buffer");

		LD_CORE_TRACE("Created Shader '{0}'", m_Name);

		return true;
	}

	DirectX11Shader::~DirectX11Shader()
	{
		ShutDown();
	}

	void DirectX11Shader::Bind()
	{
		auto deviceContext = DirectX11API::Get()->GetDeviceContext();

		deviceContext->VSSetShader(m_VertexShader, 0, 0);
		deviceContext->PSSetShader(m_PixelShader, 0, 0);
		deviceContext->IASetInputLayout(m_InputLayout);
		deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		deviceContext->VSSetConstantBuffers(0, 1, &m_ViewProjectionBuffer);
		deviceContext->VSSetConstantBuffers(1, 1, &m_ModelMatrixBuffer);

		deviceContext->VSSetConstantBuffers(2, 1, &m_MaterialBuffer);
		deviceContext->PSSetConstantBuffers(0, 1, &m_MaterialBuffer);
	}

	void DirectX11Shader::SetViewProjectionMatrix(const DirectX::XMFLOAT4X4& matrix)
	{
		D3D11_MAPPED_SUBRESOURCE mapped = {};
		HRESULT hr = DirectX11API::Get()->GetDeviceContext()->Map(m_ViewProjectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mapped);
		LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to Map MVP Constant Buffer");
		memcpy(mapped.pData, &matrix, sizeof(matrix));
		DirectX11API::Get()->GetDeviceContext()->Unmap(m_ViewProjectionBuffer, 0);
	}

	void DirectX11Shader::SetModelMatrix(const DirectX::XMFLOAT4X4& matrix)
	{
		D3D11_MAPPED_SUBRESOURCE mapped = {};
		HRESULT hr = DirectX11API::Get()->GetDeviceContext()->Map(m_ModelMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mapped);
		LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to Map MVP Constant Buffer");
		memcpy(mapped.pData, &matrix, sizeof(matrix));
		DirectX11API::Get()->GetDeviceContext()->Unmap(m_ModelMatrixBuffer, 0);
	}

	void DirectX11Shader::UploadMaterialData(void* data)
	{
		D3D11_MAPPED_SUBRESOURCE mapped = {};
		DirectX11API::Get()->GetDeviceContext()->Map(m_MaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mapped);
		memcpy(mapped.pData, data, m_MaterialDataLayout.GetStride());
		DirectX11API::Get()->GetDeviceContext()->Unmap(m_MaterialBuffer, 0);
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
		CHECK_AND_RELEASE_COMPTR(m_VertexShader);
		CHECK_AND_RELEASE_COMPTR(m_PixelShader);
		CHECK_AND_RELEASE_COMPTR(m_InputLayout);

		CHECK_AND_RELEASE_COMPTR(m_ViewProjectionBuffer);
		CHECK_AND_RELEASE_COMPTR(m_ModelMatrixBuffer);
		CHECK_AND_RELEASE_COMPTR(m_MaterialBuffer);

		LD_CORE_TRACE("Deleted Shader: {0}", m_Name);
	}

}