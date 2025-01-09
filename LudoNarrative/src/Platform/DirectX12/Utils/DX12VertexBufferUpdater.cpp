#include "ldpch.h"
#include "DX12VertexBufferUpdater.h"

#include "Ludo/Core/Application.h"

#include "Platform/DirectX12/DirectX12API.h"

namespace Ludo {

    Scope<DX12VertexBufferUpdater> DX12VertexBufferUpdater::Create(VertexBuffer::VBUpdateFrequency updateFrequency)
    {
        switch (updateFrequency)
        {
        case Ludo::VertexBuffer::IMMUTABLE:
            return CreateScope<DX12VertexBufferUpdater>();

        case Ludo::VertexBuffer::MUTABLE:
            return CreateScope<DX12MutableVertexBufferUpdater>();

        case Ludo::VertexBuffer::DYNAMIC:
            return CreateScope<DX12DynamicVertexBufferUpdater>();
        }

        LD_CORE_ASSERT(false, "Unknown Update Frequency specified");
        return nullptr;
    }

    // ========== Mutable Vertex Buffer ==========
    bool DX12MutableVertexBufferUpdater::Init(size_t bufferSize)
    {
        LD_PROFILE_FUNCTION();

        return m_UploadBuffer.Init(bufferSize);
    }

    void DX12MutableVertexBufferUpdater::UpdateVertexBuffer(ID3D12Resource2* &const vertexBuffer, uint32_t destOffset, void* data, size_t dataSize)
    {
        LD_PROFILE_RENDERER_FUNCTION();

        auto& commandList = DirectX12API::Get()->GetCommandList();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = vertexBuffer;
        barrier.Transition.Subresource = 0;

        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

        commandList->ResourceBarrier(1, &barrier);
        m_UploadBuffer.QueuUploadData(vertexBuffer, data, destOffset, dataSize);
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        commandList->ResourceBarrier(1, &barrier);
    }

    // ========== Dynamic Vertex Buffer ===========
    bool DX12DynamicVertexBufferUpdater::Init(size_t bufferSize)
    {
        LD_PROFILE_FUNCTION();

        m_UploadBuffers.reserve(2);

        for (uint32_t i = 0; i < 2; i++)
        {
            m_UploadBuffers.emplace_back(new DX12UploadBuffer)->Init(bufferSize);
        }

        m_CurrentFrame = Application::Get().GetCurrentFrame();

        return true;
    }

    DX12DynamicVertexBufferUpdater::~DX12DynamicVertexBufferUpdater()
    {
        for (auto& upBuffer : m_UploadBuffers)
        {
            delete upBuffer;
        }
    }

    void DX12DynamicVertexBufferUpdater::UpdateVertexBuffer(ID3D12Resource2*& const vertexBuffer, uint32_t destOffset, void* data, size_t dataSize)
    {
        LD_PROFILE_RENDERER_FUNCTION();

        auto frame = Application::Get().GetCurrentFrame();
        if (m_CurrentFrame != frame)
        {
            m_CurrentFrame = frame;
            m_UploadCounter = 0;
        }

        // TODO: Make so that the uploader can also decrese m_UploadBuffers size if it notices thta it doenst need all of them
        auto a = m_UploadBuffers.size();
        if (!(m_UploadCounter < m_UploadBuffers.size()))
        {
            m_UploadBuffers.emplace_back(new DX12UploadBuffer)->Init(m_UploadBuffers[0]->GetSize());
        }

        auto& commandList = DirectX12API::Get()->GetCommandList();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = vertexBuffer;
        barrier.Transition.Subresource = 0;

        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

        commandList->ResourceBarrier(1, &barrier);
        m_UploadBuffers[m_UploadCounter]->QueuUploadData(vertexBuffer, data, destOffset, dataSize);
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        commandList->ResourceBarrier(1, &barrier);

        m_UploadCounter++;
    }

}