#include "ldpch.h"
#include "DirectX11Shader.h"

#include "DirectX11API.h"
#include "Utils/DirectX11Utils.h"

namespace Ludo {

	DirectX11Shader::DirectX11Shader(const LUDO_SHADER_DESC& desc)
		: m_VertexBufferLayout(desc.VertexBufferLayout), m_MaterialDataLayout(desc.MaterialDataLayout)
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

		D3D11_MAPPED_SUBRESOURCE mapped = {};
		hr = DirectX11API::Get()->GetDeviceContext()->Map(m_MaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mapped);
		VALIDATE_DX_HRESULT(hr, "failed to map Material Buffer");
		float color[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
		memcpy(mapped.pData, color, sizeof(float) * 4);
		DirectX11API::Get()->GetDeviceContext()->Unmap(m_MaterialBuffer, 0);

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

		LD_CORE_TRACE("Deleted Shader");
	}

}