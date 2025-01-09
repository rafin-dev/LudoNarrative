#pragma once

#include "Ludo/Renderer/ImGuiTexture.h"

#include <d3d12.h>

namespace Ludo {

	class DirectX12ImGuiTexture : public ImGuiTexture
	{
	public:
		DirectX12ImGuiTexture(const Ref<Texture2D>& texture);
		~DirectX12ImGuiTexture() override;

		ImTextureID GetImTextureID() override { return m_ImTextureID; }

	private:
		Ref<Texture2D> m_Texture;
		ImTextureID m_ImTextureID;

		D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE m_GpuHandle;
	};

}