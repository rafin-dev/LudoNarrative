#pragma once

#include <d3d12.h>
#include <cstdint>

#include "Ludo/Renderer/Buffer.h"

#include "Platform/DirectX12/Utils/DX12UploadBuffer.h"

namespace Ludo {

	class DX12VertexBufferUpdater
	{
	public:
		virtual bool Init(size_t bufferSize) { return true; }
		virtual ~DX12VertexBufferUpdater() = default;

		virtual void UpdateVertexBuffer(ID3D12Resource2* &const vertexBuffer, uint32_t destOffset, void* data, size_t dataSize) {}

		static Scope<DX12VertexBufferUpdater> Create(VertexBuffer::VBUpdateFrequency updateFrequency);
	};

	typedef DX12VertexBufferUpdater DX12ImutableVertexBufferUpdater;

	class DX12MutableVertexBufferUpdater : public DX12VertexBufferUpdater
	{
	public:
		bool Init(size_t bufferSize) override;
		~DX12MutableVertexBufferUpdater() override {}

		void UpdateVertexBuffer(ID3D12Resource2* &const vertexBuffer, uint32_t destOffset, void* data, size_t dataSize) override;

	private:
		DX12UploadBuffer m_UploadBuffer;
	};

	class DX12DynamicVertexBufferUpdater : public DX12VertexBufferUpdater
	{
	public:
		virtual bool Init(size_t bufferSize) override;
		~DX12DynamicVertexBufferUpdater() override;

		virtual void UpdateVertexBuffer(ID3D12Resource2* &const vertexBuffer, uint32_t destOffset, void* data, size_t dataSize) override;

	private:
		std::vector<DX12UploadBuffer*> m_UploadBuffers;
		uint32_t m_UploadCounter = 0;
		uint32_t m_CurrentFrame = 0;
	};

}