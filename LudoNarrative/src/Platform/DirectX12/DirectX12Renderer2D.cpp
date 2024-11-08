#include "ldpch.h"

#include "Platform/DirectX12/DirectX12Renderer2D.h"
#include "Platform/DirectX12/DirectX12Renderer.h"
#include "Platform/DirectX12/DX12Utils.h"

#include "Ludo/Renderer/Shader.h"

namespace Ludo {

	bool DirectX12Renderer2D::Init()
	{
		HRESULT hr = S_OK;
		auto& device = DirectX12Renderer::Get()->GetDevice();

		auto GetShaderBlob = [](const std::string& name) -> std::pair<void*, size_t>
			{
				void* data = nullptr;
				size_t size = 0;

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
					data = malloc(size);
					if (data != nullptr)
					{
						shaderIn.read((char*)data, size);
					}
				}

				return std::pair<void*, size_t>(data, size);
			};

		auto rootSigShader = GetShaderBlob("RootSignature.cso");
		auto vertexShader = GetShaderBlob("VertexShader.cso");
		auto pixelShader = GetShaderBlob("PixelShader.cso");

		// Root Signature
		hr = device->CreateRootSignature(0, rootSigShader.first, rootSigShader.second, IID_PPV_ARGS(&m_RootSignature));
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to create Root Signature");

		// Input Assembler Data Layout 
		D3D12_INPUT_ELEMENT_DESC vertexLayout[] =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		// Pipeline State Description
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDescription = {};

		// Root Signature
		pipelineStateDescription.pRootSignature = m_RootSignature;
		// Input Layout
		pipelineStateDescription.InputLayout.NumElements = 1;
		pipelineStateDescription.InputLayout.pInputElementDescs = vertexLayout;
		pipelineStateDescription.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		// Vertex shader
		pipelineStateDescription.VS.BytecodeLength = vertexShader.second;
		pipelineStateDescription.VS.pShaderBytecode = vertexShader.first;
		// Pixel shader
		pipelineStateDescription.PS.BytecodeLength = pixelShader.second;
		pipelineStateDescription.PS.pShaderBytecode = pixelShader.first;
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

		// Pipeline state object
		hr = device->CreateGraphicsPipelineState(&pipelineStateDescription, IID_PPV_ARGS(&m_PipelineStateObject));
		VALIDATE_DXCALL_SUCCESS(hr, "Failed to create Graphics PipelineState Object");

		if (rootSigShader.first) { free(rootSigShader.first); }
		if (vertexShader.first) { free(vertexShader.first); }
		if (pixelShader.first) { free(pixelShader.first); }

		return true;
	}

	DirectX12Renderer2D::~DirectX12Renderer2D()
	{
		ShutDown();
	}

	void DirectX12Renderer2D::ShutDown()
	{
		CHECK_AND_RELEASE_COMPTR(m_RootSignature);
		CHECK_AND_RELEASE_COMPTR(m_PipelineStateObject);
	}

	Renderer2D* Renderer2D::Get()
	{
		static DirectX12Renderer2D instance;
		return &instance;
	}

}