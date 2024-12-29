#pragma once

#include <d3d12.h>

namespace Ludo {

	class DX12SRVDescriptorHeap
	{
	public:
		DX12SRVDescriptorHeap() = default;

		// Called by the System
		bool Init();
		void ShutDown(); // No destructor as this should have unnitialization explicitly controled by the owner

		void NewFrame();
		void BindToDescriptorTable(); // Called before draw call, sets descrptor table to current offset, advances offset

		// Called by the Textures
		void CreateDescriptorInOffset(ID3D12Resource2* const& resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc, uint32_t slot);

		ID3D12DescriptorHeap* const& GetDescriptorHeap() { return m_DescriptorHeap; }

		// Called by ImGui 
		void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle);

		void Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle);

	private:
		DX12SRVDescriptorHeap(const DX12SRVDescriptorHeap&) = delete;

		ID3D12DescriptorHeap* m_DescriptorHeap = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHeapStart;
		D3D12_GPU_DESCRIPTOR_HANDLE m_GpuHeapStart;
		UINT m_IncrementAmount = 0;

		uint32_t m_CurrentOffset = 64; // ImGui manages the first 64 descriptors, so the Engine's list only start at index 64
		int32_t m_CurrentMaxSlot = -1; // -1 represents that no texture was bound

		std::vector<int> m_ImGuiFreeIndices;
	};

}