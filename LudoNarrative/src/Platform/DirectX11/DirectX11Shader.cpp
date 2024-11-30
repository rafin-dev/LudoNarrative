#include "ldpch.h"
#include "DirectX11Shader.h"

#include "DirectX11API.h"
#include "Utils/DirectX11Utils.h"

namespace Ludo {

	DirectX11Shader::DirectX11Shader(const LUDO_SHADER_DESC& desc)
		: m_VertexBufferLayout(desc.VertexBufferLayout)
	{
		Init(desc);
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

	bool DirectX11Shader::Init(const LUDO_SHADER_DESC& desc)
	{
		HRESULT hr = S_OK;
		auto device = DirectX11API::Get()->GetDevice();

		// ========== Shaders ==========
		hr = device->CreateVertexShader(desc.VertexShaderBlob, desc.VertexShaderSize, NULL, &m_VertexShader);
		VALIDATE_DX_HRESULT(hr, "Failed to create Vertex Shader");
		hr = device->CreatePixelShader(desc.PixelShaderBlob, desc.PixelShaderSize, NULL, &m_PixelShader);
		VALIDATE_DX_HRESULT(hr, "Failed to create Pixel Shader");

		// ========== Vertex Buffer Layout ==========
		std::vector<D3D11_INPUT_ELEMENT_DESC> ElementLayout;
		ElementLayout.reserve(desc.VertexBufferLayout.GetElements().size());
		for (auto& element : desc.VertexBufferLayout)
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
		hr = device->CreateInputLayout(ElementLayout.data(), ElementLayout.size(), desc.VertexShaderBlob, desc.VertexShaderSize, &m_InputLayout);
		VALIDATE_DX_HRESULT(hr, "Failed to create Input Layout");

		LD_CORE_TRACE("Created Shader");

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
	}

	void DirectX11Shader::SetViewProjectionMatrix(const DirectX::XMFLOAT4X4& matrix)
	{
	}

	void DirectX11Shader::SetModelMatrix(const DirectX::XMFLOAT4X4& matrix)
	{
	}

	void DirectX11Shader::AddEntry()
	{
	}

	void DirectX11Shader::RemoveEntry()
	{
	}

	void DirectX11Shader::UploadMaterialDataBuffer(void* data)
	{
	}

	void DirectX11Shader::NextEntry()
	{
	}

	const BufferLayout& DirectX11Shader::GetVertexBufferLayout()
	{
		return m_VertexBufferLayout;
	}

	const BufferLayout& DirectX11Shader::GetMaterialLayout()
	{
		return BufferLayout();
	}

	void DirectX11Shader::ShutDown()
	{
		CHECK_AND_RELEASE_COMPTR(m_VertexShader);
		CHECK_AND_RELEASE_COMPTR(m_PixelShader);
		CHECK_AND_RELEASE_COMPTR(m_InputLayout);
		LD_CORE_TRACE("Deleted Shader");
	}

}