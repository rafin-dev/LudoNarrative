#include "ldpch.h"
#include "DirectX12ImGuiTexture.h"

#include "Platform/DirectX12/DirectX12API.h"
#include "Platform/DirectX12/DirectX12Texture.h"

namespace Ludo {

	DirectX12ImGuiTexture::DirectX12ImGuiTexture(const Ref<Texture2D>& texture)
		: m_Texture(texture)
	{
		LD_PROFILE_FUNCTION();

		DirectX12Texture2D* d3d12Texture = dynamic_cast<DirectX12Texture2D*>(m_Texture.get());

		DirectX12API::Get()->GetSRVDescriptorHeap().Alloc(&m_CpuHandle, &m_GpuHandle);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.PlaneSlice = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		DirectX12API::Get()->GetDevice()->CreateShaderResourceView(d3d12Texture->GetResource(), &srvDesc, m_CpuHandle);

		m_ImTextureID = (ImTextureID)m_GpuHandle.ptr;
	}

	DirectX12ImGuiTexture::~DirectX12ImGuiTexture()
	{
		LD_PROFILE_FUNCTION();

		DirectX12API::Get()->GetSRVDescriptorHeap().Free(m_CpuHandle, m_GpuHandle);
	}
	
}