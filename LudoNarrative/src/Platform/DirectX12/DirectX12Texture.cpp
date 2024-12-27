#include "ldpch.h"
#include "DirectX12Texture.h"

#include "Platform/DirectX12/Utils/DX12Utils.h"
#include "Platform/DirectX12/Utils/DX12UploadBuffer.h"
#include "Platform/DirectX12/DirectX12API.h"

#include <stb_image/stb_image.h>

namespace Ludo {

	static std::atomic<uint32_t> s_IDcounter = 0;

	DirectX12Texture2D::DirectX12Texture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height), m_ID(s_IDcounter++)
	{
		LD_PROFILE_FUNCTION();

		Init();
	}

	DirectX12Texture2D::DirectX12Texture2D(const std::string& path)
		: m_Path(path), m_ID(s_IDcounter++)
	{
		LD_PROFILE_FUNCTION();

		int channelCount = 4;
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);

		stbi_uc* data = nullptr;
		{
			LD_PROFILE_SCOPE("stbi_load");

			data = stbi_load(m_Path.string().c_str(), &width, &height, &channels, channelCount);
			LD_CORE_ASSERT(data != nullptr, "Failed to load Image: {0}", m_Path.string());
			m_Width = width;
			m_Height = height;
		}

		Init();

		SetData(data, m_Height * m_Width * sizeof(uint32_t));

		stbi_image_free(data);
	}

	bool DirectX12Texture2D::Init()
	{
		LD_PROFILE_FUNCTION();

		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 0;
		heapProperties.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width = m_Width;
		textureDesc.Height = m_Height;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		HRESULT hr = DirectX12API::Get()->GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
			&textureDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_Texture));
		VALIDATE_DX12_HRESULT(hr, "Failed to create Texture of size: [{0}, {1}]", m_Width, m_Height);

		LD_CORE_TRACE("Created Texture of size: [{0}, {1}]", m_Width, m_Height);

		return true;
	}

	DirectX12Texture2D::~DirectX12Texture2D()
	{
		ShutDown();
	}

	uint32_t DirectX12Texture2D::GetWidth() const
	{
		return m_Width;
	}

	uint32_t DirectX12Texture2D::GetHeight() const
	{
		return m_Height;
	}

#define Align(x, y) std::max(((int)(x) / (int)(y)) * (int)(y), (int)(y));

	void DirectX12Texture2D::SetData(void* data, uint32_t size)
	{
		LD_CORE_ASSERT(size <= m_Height * m_Width * sizeof(uint32_t), "Atempt to write out of texture bounds");

		DX12UploadBuffer uploadBuffer;
		uploadBuffer.Init(size);
		uploadBuffer.FillBufferData(data, 0, size);

		D3D12_TEXTURE_COPY_LOCATION src = {};
		src.pResource = uploadBuffer.GetBuffer();
		src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		src.PlacedFootprint.Offset = 0;
		src.PlacedFootprint.Footprint.Width = m_Width;
		src.PlacedFootprint.Footprint.Height = m_Height;
		src.PlacedFootprint.Footprint.Depth = 1;
		src.PlacedFootprint.Footprint.RowPitch = Align(m_Width * sizeof(uint32_t), D3D12_TEXTURE_DATA_PITCH_ALIGNMENT); // Assure the row Pitch is a multiple of 256
		src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		D3D12_TEXTURE_COPY_LOCATION dest = {};
		dest.pResource = m_Texture;
		dest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dest.SubresourceIndex = 0;

		D3D12_BOX srcBox = {};
		srcBox.left = 0;
		srcBox.top = 0;
		srcBox.front = 0;
		srcBox.right = m_Width;
		srcBox.bottom = m_Height;
		srcBox.back = 1;

		DirectX12API::Get()->ImediateCopyTextureRegion(dest, 0, 0, 0, src, srcBox);
	}

	void DirectX12Texture2D::Bind(uint32_t slot) const
	{
		LD_PROFILE_FUNCTION();


		D3D12_SHADER_RESOURCE_VIEW_DESC srv = {};
		srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srv.Texture2D.MipLevels = 1;
		srv.Texture2D.MostDetailedMip = 0;
		srv.Texture2D.PlaneSlice = 0;
		srv.Texture2D.ResourceMinLODClamp = 0.0f;

		UINT stride = DirectX12API::Get()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		auto cpuHandle = DirectX12API::Get()->GetTexturesDecriptorHeap()->GetCPUDescriptorHandleForHeapStart();
		cpuHandle.ptr += stride * slot;

		DirectX12API::Get()->GetDevice()->CreateShaderResourceView(m_Texture, &srv, cpuHandle);
	}

	void DirectX12Texture2D::ShutDown()
	{
		LD_PROFILE_FUNCTION();

		CHECK_AND_RELEASE_COMPTR(m_Texture);
	}

}