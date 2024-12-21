#include "ldpch.h"
#include "DirectX11Texture.h"

#include "DirectX11API.h"
#include "Utils/DirectX11Utils.h"

#include <stb_image.h>

namespace Ludo {

	DirectX11Texture2D::DirectX11Texture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		LD_PROFILE_FUNCTION();

		Init(nullptr, 4);
	}

	DirectX11Texture2D::DirectX11Texture2D(const std::string& path)
		: m_Path(path)
	{
		LD_PROFILE_FUNCTION();

		int channelCount = 4;
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);

		stbi_uc* data = nullptr;
		{
			LD_PROFILE_SCOPE("stbi_load - DirectX11Texture2D::DirectX11Texture2D(const std::string&)");

			data = stbi_load(m_Path.c_str(), &width, &height, &channels, channelCount);
			LD_CORE_ASSERT(data != nullptr, "Failed to load Image: {0}", m_Path);
			m_Width = width;
			m_Height = height;
		}

		Init(data, channelCount);
		stbi_image_free(data);
	}

	DirectX11Texture2D::~DirectX11Texture2D()
	{
		ShutDown();
	}

	bool DirectX11Texture2D::Init(void* data, int channelCount)
	{
		LD_PROFILE_FUNCTION();

		HRESULT hr = S_OK;
		auto device = DirectX11API::Get()->GetDevice();

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = m_Width;
		textureDesc.Height = m_Height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DYNAMIC;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA TextureSubresourceData = {};
		TextureSubresourceData.pSysMem = data;
		TextureSubresourceData.SysMemPitch = m_Width * channelCount;

		hr = device->CreateTexture2D(&textureDesc, data != nullptr ? &TextureSubresourceData : nullptr, &m_Texture);
		VALIDATE_DX_HRESULT(hr, "Failed to create Texture: {0} [{1}, {2}]", m_Path, m_Width, m_Height);

		hr = device->CreateShaderResourceView(m_Texture, nullptr, &m_ShaderResourceView);
		VALIDATE_DX_HRESULT(hr, "Failed to create Shader Resource View for texture: {0}, [{1}, {2}]", m_Path, m_Width, m_Height);

		D3D11_SAMPLER_DESC sampleDesc = {};
		sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDesc.MipLODBias = 0.0f;
		sampleDesc.MaxAnisotropy = 1;
		sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampleDesc.BorderColor[0] = 1.0f;
		sampleDesc.BorderColor[1] = 1.0f;
		sampleDesc.BorderColor[2] = 1.0f;
		sampleDesc.BorderColor[3] = 1.0f;
		sampleDesc.MinLOD = -FLT_MAX;
		sampleDesc.MaxLOD = FLT_MAX;

		hr = device->CreateSamplerState(&sampleDesc, &m_SamplerState);
		VALIDATE_DX_HRESULT(hr, "Failed to create Sampler State for Texture: {0} [{1}, {2}]", m_Path, m_Width, m_Height);

		LD_CORE_TRACE("Loaded Texture: {0} [{1}, {2}]", m_Path, m_Width, m_Height);

		return true;
	}

	void DirectX11Texture2D::ShutDown()
	{
		LD_PROFILE_FUNCTION();

		CHECK_AND_RELEASE_COMPTR(m_Texture);
		CHECK_AND_RELEASE_COMPTR(m_ShaderResourceView);
		CHECK_AND_RELEASE_COMPTR(m_SamplerState);
		LD_CORE_TRACE("Unloaded Texture: {0} [{1}, {2}]", m_Path, m_Width, m_Height);
	}

	void DirectX11Texture2D::SetData(void* data, uint32_t size)
	{
		LD_PROFILE_FUNCTION();

		LD_CORE_ASSERT(size == m_Width * m_Height * 4, "Data Size does not match texture Size! Data Size: {0}, Texture Size: {1}", size, m_Width * m_Height * 4);
		D3D11_MAPPED_SUBRESOURCE mapped = {};
		HRESULT hr = DirectX11API::Get()->GetDeviceContext()->Map(m_Texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		LD_CORE_ASSERT(SUCCEEDED(hr), "Failed to map Subresource");

		memcpy(mapped.pData, data, size);

		DirectX11API::Get()->GetDeviceContext()->Unmap(m_Texture, 0);
	}

	void DirectX11Texture2D::Bind(uint32_t slot = 0) const
	{
		LD_PROFILE_FUNCTION();

		auto deviceContext = DirectX11API::Get()->GetDeviceContext();

		deviceContext->PSSetShaderResources(slot, 1, &m_ShaderResourceView);
		deviceContext->PSSetSamplers(slot, 1, &m_SamplerState);
	}

}