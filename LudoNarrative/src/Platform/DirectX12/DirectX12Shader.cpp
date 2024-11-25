#include "ldpch.h"
#include "DirectX12Shader.h"

#include "DirectX12API.h"
#include "Utils/DX12Utils.h"

#include "Ludo/Application.h"

namespace Ludo {

	class ShaderBlob
	{
	public:
		ShaderBlob(const std::string& name)
		{
			wchar_t moduleFileName[MAX_PATH];
			GetModuleFileNameW(nullptr, moduleFileName, MAX_PATH);

			std::filesystem::path shaderPath = moduleFileName;
			shaderPath.remove_filename();
			shaderPath += "../LudoNarrative";
			shaderPath = shaderPath / name;
			std::ifstream shaderIn(shaderPath, std::ios::binary);

			if (shaderIn.is_open())
			{
				size = std::filesystem::file_size(shaderPath);
				blob = malloc(size);
				if (blob != nullptr)
				{
					shaderIn.read((char*)blob, size);
				}
			}
		}
		~ShaderBlob()
		{
			if (blob != nullptr)
			{
				free(blob);
			}
		}

		void* blob = nullptr;
		size_t size = 0;
	};

	ID3D12RootSignature* DirectX12Shader::m_2DRootSignature = nullptr;
	std::array<D3D12_INPUT_ELEMENT_DESC, 1> DirectX12Shader::m_2DElementLayout;

	DirectX12Shader::DirectX12Shader(const LUDO_SHADER_DESC& desc)
	{
		LD_CORE_ASSERT(desc.VertexBufferLayout.GetElements().size() != 0, "D3D12 Shader cannot be created without Vertex Buffer Layout");
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

	bool DirectX12Shader::Init(const LUDO_SHADER_DESC& desc)
	{
		HRESULT hr = S_OK;
		auto& device = DirectX12API::Get()->GetDevice();

		m_VertexBufferLayout = desc.VertexBufferLayout;
		m_MaterialLayout = desc.MaterialDataLayout;

		std::vector<D3D12_INPUT_ELEMENT_DESC> ElementLayout;
		ElementLayout.reserve(desc.VertexBufferLayout.GetElements().size());

		for (auto& element : desc.VertexBufferLayout)
		{
			D3D12_INPUT_ELEMENT_DESC desc;
			if (element.Type == ShaderDataType::Float3x3)
			{
				for (uint32_t i = 0; i < 3; i++)
				{
					desc =
					{ element.Name.c_str(), i, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
					ElementLayout.push_back(desc);
				}
			}
			else if (element.Type == ShaderDataType::Float4x4)
			{
				for (uint32_t i = 0; i < 4; i++)
				{
					desc =
					{ element.Name.c_str(), i, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
					ElementLayout.push_back(desc);
				}
			}

			desc =
			{ element.Name.c_str(), 0, GetDxgiFormatFromShaderDataType(element.Type), 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
			ElementLayout.push_back(desc);
		}

		m_CBVsize = desc.MaterialDataLayout.GetStride();

		// Pipeline State
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDescription = {};

		// Root Signature
		pipelineStateDescription.pRootSignature = m_2DRootSignature;

		// Input Layout
		pipelineStateDescription.InputLayout.NumElements = ElementLayout.size();
		pipelineStateDescription.InputLayout.pInputElementDescs = ElementLayout.data();
		pipelineStateDescription.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		
		// Vertex shader
		pipelineStateDescription.VS.BytecodeLength = desc.VertexShaderSize;
		pipelineStateDescription.VS.pShaderBytecode = desc.VertexShaderBlob;
		
		// Pixel shader
		pipelineStateDescription.PS.BytecodeLength = desc.PixelShaderSize;
		pipelineStateDescription.PS.pShaderBytecode = desc.PixelShaderBlob;
		
		// Domain shader
		pipelineStateDescription.DS.BytecodeLength = 0;
		pipelineStateDescription.DS.pShaderBytecode = nullptr;
		
		// Hull shader
		pipelineStateDescription.HS.BytecodeLength = 0;
		pipelineStateDescription.HS.pShaderBytecode = nullptr;
		
		// Geometry shader
		pipelineStateDescription.GS.BytecodeLength = 0;
		pipelineStateDescription.GS.pShaderBytecode = nullptr;
		
		// Rasterizer
		pipelineStateDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateDescription.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		pipelineStateDescription.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		pipelineStateDescription.RasterizerState.FrontCounterClockwise = FALSE;
		pipelineStateDescription.RasterizerState.DepthBias = 0;
		pipelineStateDescription.RasterizerState.DepthBiasClamp = 0.0f;
		pipelineStateDescription.RasterizerState.SlopeScaledDepthBias = 0.0f;
		pipelineStateDescription.RasterizerState.DepthClipEnable = FALSE;
		pipelineStateDescription.RasterizerState.MultisampleEnable = FALSE;
		pipelineStateDescription.RasterizerState.AntialiasedLineEnable = FALSE;
		pipelineStateDescription.RasterizerState.ForcedSampleCount = 0;
		
		// StreamOutput
		pipelineStateDescription.StreamOutput.pSODeclaration = nullptr;
		pipelineStateDescription.StreamOutput.NumEntries = 0;
		pipelineStateDescription.StreamOutput.pBufferStrides = nullptr;
		pipelineStateDescription.StreamOutput.NumStrides = 0;
		pipelineStateDescription.StreamOutput.RasterizedStream = 0;
		
		// NumRenderTargets
		pipelineStateDescription.NumRenderTargets = 1;
		
		// RTVFormats
		pipelineStateDescription.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		
		// DSVFormat
		pipelineStateDescription.DSVFormat = DXGI_FORMAT_UNKNOWN;
		
		// BlendState
		pipelineStateDescription.BlendState.AlphaToCoverageEnable = FALSE;
		pipelineStateDescription.BlendState.IndependentBlendEnable = FALSE;
		
		// RenderTarget BlendState
		pipelineStateDescription.BlendState.RenderTarget[0].BlendEnable = TRUE;
		pipelineStateDescription.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		pipelineStateDescription.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		pipelineStateDescription.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		pipelineStateDescription.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
		pipelineStateDescription.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		pipelineStateDescription.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		pipelineStateDescription.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
		pipelineStateDescription.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
		pipelineStateDescription.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		
		// DepthStencilState
		pipelineStateDescription.DepthStencilState.DepthEnable = FALSE;
		pipelineStateDescription.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		pipelineStateDescription.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		pipelineStateDescription.DepthStencilState.StencilEnable = FALSE;
		pipelineStateDescription.DepthStencilState.StencilReadMask = 0;
		pipelineStateDescription.DepthStencilState.StencilWriteMask = 0;
		pipelineStateDescription.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		pipelineStateDescription.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		pipelineStateDescription.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		pipelineStateDescription.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		pipelineStateDescription.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		pipelineStateDescription.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		pipelineStateDescription.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		pipelineStateDescription.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		
		// SampleMask
		pipelineStateDescription.SampleMask = 0xFFFFFFFF;
		
		// SampleDesc
		pipelineStateDescription.SampleDesc.Count = 1;
		pipelineStateDescription.SampleDesc.Quality = 0;
		
		// NodeMask
		pipelineStateDescription.NodeMask = 0;
		
		// Chached PSO
		pipelineStateDescription.CachedPSO.CachedBlobSizeInBytes = 0;
		pipelineStateDescription.CachedPSO.pCachedBlob = nullptr;
		
		// Flags
		pipelineStateDescription.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		hr = device->CreateGraphicsPipelineState(&pipelineStateDescription, IID_PPV_ARGS(&m_PipelineStateObject));
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to create Graphics PipelineState Object");

		return true;
	}

	DirectX12Shader::~DirectX12Shader()
	{
		ShutDown();
	}

	void DirectX12Shader::Bind()
	{
		m_CBVswapChain.Swap();
		auto& CBV = m_CBVswapChain.GetCurrentItem();
		m_CurrentEntry = 0;
		if (CBV.BufferSize != m_CBVsize * m_ShaderEntriesCount)
		{
			CHECK_AND_RELEASE_COMPTR(CBV.Buffer);

			// ========== Heap & Resource properties ==========
			D3D12_HEAP_PROPERTIES heapProperties = {};
			heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
			heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heapProperties.CreationNodeMask = 0;
			heapProperties.VisibleNodeMask = 0;
			
			D3D12_RESOURCE_DESC resourceDescription = {};
			resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			resourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
			resourceDescription.Width = m_CBVsize * m_ShaderEntriesCount;
			resourceDescription.Height = 1;
			resourceDescription.DepthOrArraySize = 1;
			resourceDescription.MipLevels = 1;
			resourceDescription.Format = DXGI_FORMAT_UNKNOWN;
			resourceDescription.SampleDesc.Count = 1;
			resourceDescription.SampleDesc.Quality = 0;
			resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

			DirectX12API::Get()->GetDevice()->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&resourceDescription,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&CBV.Buffer)
			);
			CBV.BufferSize = resourceDescription.Width;

			D3D12_RANGE range = { 0, 0 };
			CBV.Buffer->Map(0, &range, &CBV.MappedBuffer);
		}

		auto& commandList = DirectX12API::Get()->GetCommandList();

		commandList->SetPipelineState(m_PipelineStateObject);
		commandList->SetGraphicsRootSignature(m_2DRootSignature);
		uint32_t size[] = { Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight() };
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = m_CBVswapChain.GetCurrentItem().Buffer->GetGPUVirtualAddress();
		gpuAddress += m_CBVsize * m_CurrentEntry;

		commandList->SetGraphicsRootConstantBufferView(1, gpuAddress);
	}

	void DirectX12Shader::SetViewProjectionMatrix(const DirectX::XMFLOAT4X4& matrix)
	{
		DirectX12API::Get()->GetCommandList()->SetGraphicsRoot32BitConstants(0, 16, &matrix, 0);
	}

	void DirectX12Shader::SetModelMatrix(const DirectX::XMFLOAT4X4& matrix)
	{
		DirectX12API::Get()->GetCommandList()->SetGraphicsRoot32BitConstants(0, 16, &matrix, 16);
	}

	void DirectX12Shader::AddEntry()
	{
		m_ShaderEntriesCount++;
	}

	void DirectX12Shader::RemoveEntry()
	{
		m_ShaderEntriesCount--;
	}

	void DirectX12Shader::NextEntry()
	{
		m_CurrentEntry++;
		D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = m_CBVswapChain.GetCurrentItem().Buffer->GetGPUVirtualAddress();
		gpuAddress += m_CBVsize * m_CurrentEntry;
		DirectX12API::Get()->GetCommandList()->SetGraphicsRootConstantBufferView(1, gpuAddress);
	}

	void DirectX12Shader::UploadMaterialDataBuffer(void* data)
	{
		uint8_t* dest = (uint8_t*)m_CBVswapChain.GetCurrentItem().MappedBuffer;
		dest += m_CBVsize * m_CurrentEntry;

		memcpy(dest, data, m_CBVsize);
	}

	bool DirectX12Shader::InitSystem()
	{
		ShaderBlob RootSigBlob("RootSignature.cso");

		HRESULT hr = DirectX12API::Get()->GetDevice()->CreateRootSignature(0, RootSigBlob.blob, RootSigBlob.size, IID_PPV_ARGS(&m_2DRootSignature));
		if (FAILED(hr))
		{
			LD_CORE_ERROR("Failed to create Root Signature for the 2D Pipeline");
			CloseSystem();
			return false;
		}

		m_2DElementLayout =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		return true;
	}

	void DirectX12Shader::CloseSystem()
	{
		CHECK_AND_RELEASE_COMPTR(m_2DRootSignature);
	}

	void DirectX12Shader::ShutDown()
	{
		DeleteCBVBuffers();

		CHECK_AND_RELEASE_COMPTR(m_PipelineStateObject);
		CHECK_AND_RELEASE_COMPTR(m_RootSignature);
	}

	inline bool DirectX12Shader::CreateCBVBuffers()
	{
		// ========== Heap & Resource properties ==========
		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 0;
		heapProperties.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC resourceDescription = {};
		resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDescription.Width = m_CBVsize * m_ShaderEntriesCount;
		resourceDescription.Height = 1;
		resourceDescription.DepthOrArraySize = 1;
		resourceDescription.MipLevels = 1;
		resourceDescription.Format = DXGI_FORMAT_UNKNOWN;
		resourceDescription.SampleDesc.Count = 1;
		resourceDescription.SampleDesc.Quality = 0;
		resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

		bool success = true;

		m_CBVswapChain.ForEachElement([heapProperties, resourceDescription, success](CBVbuffer& buffer) mutable {
			if (resourceDescription.Width == 0)
			{
				return;
			}

			HRESULT hr = DirectX12API::Get()->GetDevice()->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&resourceDescription,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&buffer.Buffer)
				);
			if (FAILED(hr))
			{
				success = false;
				return;
			}
			buffer.BufferSize = resourceDescription.Width;

			D3D12_RANGE range = { 0, 0 };
			hr = buffer.Buffer->Map(0, &range, &buffer.MappedBuffer);
			success = SUCCEEDED(hr);
		});

		return success;
	}

	inline void DirectX12Shader::DeleteCBVBuffers()
	{
		m_CBVswapChain.ForEachElement([](CBVbuffer& buffer) {

			CHECK_AND_RELEASE_COMPTR(buffer.Buffer);
			buffer.MappedBuffer = nullptr;

		});
	}
}