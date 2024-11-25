#pragma once

#include "ldpch.h"
#include "Ludo/Renderer/Shader.h"
#include "Utils/DirectX12UploadBuffer.h"
#include "Utils/GenericSwapChain.h"

namespace Ludo {

	class DirectX12Shader : public Shader
	{
	public:
		DirectX12Shader(const LUDO_SHADER_DESC& desc);
		bool Init(const LUDO_SHADER_DESC& desc);
		virtual ~DirectX12Shader() override;

		virtual void Bind() override;

		virtual void SetViewProjectionMatrix(const DirectX::XMFLOAT4X4& matrix) override;
		virtual void SetModelMatrix(const DirectX::XMFLOAT4X4& matrix) override;

		virtual void AddEntry() override;
		virtual void RemoveEntry() override;

		virtual void NextEntry() override;

		virtual void UploadMaterialDataBuffer(void* data) override;

		virtual const BufferLayout& GetVertexBufferLayout() override { return m_VertexBufferLayout; }
		virtual const BufferLayout& GetMaterialLayout() override { return m_MaterialLayout; }

		static bool InitSystem();
		static void CloseSystem();

	private:
		void ShutDown();
		inline bool CreateCBVBuffers();
		inline void DeleteCBVBuffers();

		BufferLayout m_VertexBufferLayout;
		BufferLayout m_MaterialLayout;

		ID3D12PipelineState* m_PipelineStateObject = nullptr;
		ID3D12RootSignature* m_RootSignature = nullptr;

		uint32_t m_CurrentEntry = 0;

		// CBV

		struct CBVbuffer
		{
			ID3D12Resource2* Buffer = nullptr;
			void* MappedBuffer = nullptr;
			size_t BufferSize = 0;
		};

		GenericSwapChain<CBVbuffer, 2> m_CBVswapChain;

		size_t m_CBVsize = 0;

		std::atomic<uint32_t> m_ShaderEntriesCount = 0;
		
		static ID3D12RootSignature* m_2DRootSignature;
		static std::array<D3D12_INPUT_ELEMENT_DESC, 1> m_2DElementLayout;
	};

}