#include "ldpch.h"
#include "DirectX12Shader.h"

#include "Platform/DirectX12/Utils/DX12Utils.h"
#include "Platform/DirectX12/DirectX12API.h"
#include "Platform/DirectX12/DirectX12FrameBuffer.h"

namespace Ludo {

	ID3D12RootSignature* DirectX12Shader::s_RootSignature = nullptr;
	IDxcUtils* DirectX12Shader::s_DxCompilerUtils = nullptr;
	IDxcCompiler3* DirectX12Shader::s_DxCompiler = nullptr;

	DirectX12Shader::DirectX12Shader(const std::string& name, void* vertexShaderBuffer, size_t vertexShaderSize, void* pixelShaderBuffer, size_t pixelShaderSize, const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout)
		: m_Name(name), m_VertexBufferLayout(vertexLayout), m_MaterialLayout(materialDataLayout)
	{
		LD_PROFILE_FUNCTION();

		m_Shaders =
		{
			{ DX12ShaderCompiler::VertexShader, nullptr },
			{ DX12ShaderCompiler::PixelShader, nullptr }
		};

		HRESULT hr = DX12ShaderCompiler::CreateBlob(vertexShaderSize, &m_Shaders[DX12ShaderCompiler::VertexShader]);
		CHECK_DX12_HRESULT(hr, "Failed to create Shader Blob");
		memcpy(m_Shaders[DX12ShaderCompiler::VertexShader]->GetBufferPointer(), vertexShaderBuffer, vertexShaderSize);

		hr = DX12ShaderCompiler::CreateBlob(pixelShaderSize, &m_Shaders[DX12ShaderCompiler::PixelShader]);
		CHECK_DX12_HRESULT(hr, "Failed to create Shader Blob");
		memcpy(m_Shaders[DX12ShaderCompiler::PixelShader]->GetBufferPointer(), pixelShaderBuffer, pixelShaderSize);

		Init(m_Shaders);
	}

	DirectX12Shader::DirectX12Shader(const std::string& name, const std::filesystem::path& shaderSrcPath, const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout)
		: m_Name(name), m_VertexBufferLayout(vertexLayout), m_MaterialLayout(materialDataLayout)
	{
		LD_PROFILE_FUNCTION();
	
		std::unordered_map<DX12ShaderCompiler::ShaderKind, std::string> shadersSources;
		std::string source;

		ReadFile(shaderSrcPath, source);
		ParseShaders(source, shadersSources);
		CompileShaders(shadersSources, m_Shaders);
		Init(m_Shaders);
	}

	DirectX12Shader::~DirectX12Shader()
	{
		ShutDown();

		LD_CORE_TRACE("Deleted Shader");
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

		LD_CORE_ASSERT(false, "Unknown Shader Data Type");
		return DXGI_FORMAT_UNKNOWN;
	}

	bool DirectX12Shader::Init(std::unordered_map<DX12ShaderCompiler::ShaderKind, IDxcBlob*> shaders)
	{
		LD_PROFILE_FUNCTION();

		HRESULT hr = S_OK;
		auto& device = DirectX12API::Get()->GetDevice();

		// If theres no material, ignore the upload buffer creation
		// Shoudnt be an issue... in theory
		if (m_MaterialLayout.GetElements().size() > 0)
		{
			m_ShaderResources.emplace_back().Init(m_MaterialLayout.GetStride());
		}
		
		m_ElementLayout.reserve(m_VertexBufferLayout.GetElements().size());

		// TODO: m_CurrentFrame = Application::Get().GetFrameCount();

		for (auto& element : m_VertexBufferLayout)
		{
			D3D12_INPUT_ELEMENT_DESC desc;
			if (element.Type == ShaderDataType::Float3x3)
			{
				for (uint32_t i = 0; i < 3; i++)
				{
					desc =
					{ element.Name.c_str(), i, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
					m_ElementLayout.push_back(desc);
				}
			}
			else if (element.Type == ShaderDataType::Float4x4)
			{
				for (uint32_t i = 0; i < 4; i++)
				{
					desc =
					{ element.Name.c_str(), i, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
					m_ElementLayout.push_back(desc);
				}
			}

			desc =
			{ element.Name.c_str(), 0, GetDxgiFormatFromShaderDataType(element.Type), 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
			m_ElementLayout.push_back(desc);
		}

		LD_CORE_TRACE("Created Shader");

		return true;
	}

	void DirectX12Shader::ShutDown()
	{
		LD_PROFILE_FUNCTION();

		for (auto& shader : m_Shaders)
		{
			CHECK_AND_RELEASE_COMPTR(shader.second);
		}
		for (auto& pso : m_PipelineStates)
		{
			CHECK_AND_RELEASE_COMPTR(pso.second);
		}
	}

	namespace Utils {
		static bool IsBlendValid(DXGI_FORMAT format)
		{
			switch (format)
			{
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
				return true;
			case DXGI_FORMAT_R32G32B32_FLOAT:
				return true;
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
				return true;
			case DXGI_FORMAT_R16G16B16A16_UNORM:
				return true;
			case DXGI_FORMAT_R16G16B16A16_SNORM:
				return true;
			case DXGI_FORMAT_R32G32_FLOAT:
				return true;
			case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
				return true;
			case DXGI_FORMAT_R10G10B10A2_UNORM:
				return true;
			case DXGI_FORMAT_R11G11B10_FLOAT:
				return true;
			case DXGI_FORMAT_R8G8B8A8_UNORM:
				return true;
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
				return true;
			case DXGI_FORMAT_R8G8B8A8_SNORM:
				return true;
			case DXGI_FORMAT_R16G16_FLOAT:
				return true;
			case DXGI_FORMAT_R16G16_UNORM:
				return true;
			case DXGI_FORMAT_R16G16_SNORM:
				return true;
			case DXGI_FORMAT_D32_FLOAT:
				return true;
			case DXGI_FORMAT_R32_FLOAT:
				return true;
			case DXGI_FORMAT_R8G8_UNORM:
				return true;
			case DXGI_FORMAT_R8G8_SNORM:
				return true;
			case DXGI_FORMAT_R16_FLOAT:
				return true;
			case DXGI_FORMAT_D16_UNORM:
				return true;
			case DXGI_FORMAT_R16_UNORM:
				return true;
			case DXGI_FORMAT_R16_SNORM:
				return true;
			case DXGI_FORMAT_R8_UNORM:
				return true;
			case DXGI_FORMAT_R8_SNORM:
				return true;
			case DXGI_FORMAT_A8_UNORM:
				return true;
			case DXGI_FORMAT_R1_UNORM:
				return true;
			case DXGI_FORMAT_R8G8_B8G8_UNORM:
				return true;
			case DXGI_FORMAT_G8R8_G8B8_UNORM:
				return true;
			case DXGI_FORMAT_BC1_UNORM:
				return true;
			case DXGI_FORMAT_BC1_UNORM_SRGB:
				return true;
			case DXGI_FORMAT_BC2_UNORM:
				return true;
			case DXGI_FORMAT_BC2_UNORM_SRGB:
				return true;
			case DXGI_FORMAT_BC3_UNORM:
				return true;
			case DXGI_FORMAT_BC3_UNORM_SRGB:
				return true;
			case DXGI_FORMAT_BC4_UNORM:
				return true;
			case DXGI_FORMAT_BC4_SNORM:
				return true;
			case DXGI_FORMAT_BC5_UNORM:
				return true;
			case DXGI_FORMAT_BC5_SNORM:
				return true;
			case DXGI_FORMAT_B5G6R5_UNORM:
				return true;
			case DXGI_FORMAT_B5G5R5A1_UNORM:
				return true;
			case DXGI_FORMAT_B8G8R8A8_UNORM:
				return true;
			case DXGI_FORMAT_B8G8R8X8_UNORM:
				return true;
			case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
				return true;
			case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
				return true;
			case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
				return true;
			case DXGI_FORMAT_BC6H_UF16:
				return true;
			case DXGI_FORMAT_BC6H_SF16:
				return true;
			case DXGI_FORMAT_BC7_UNORM:
				return true;
			case DXGI_FORMAT_BC7_UNORM_SRGB:
				return true;
			case DXGI_FORMAT_B4G4R4A4_UNORM:
				return true;
			}

			return false;
		}
	}

	ID3D12PipelineState* DirectX12Shader::CreatePSOforFrameBufferFormats(std::vector<DXGI_FORMAT>& formats)
	{
		LD_CORE_ASSERT(formats.size() == 9);

		// Pipeline State
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDescription = {};

		// Root Signature
		pipelineStateDescription.pRootSignature = s_RootSignature;

		// Input Layout
		pipelineStateDescription.InputLayout.NumElements = (UINT)m_ElementLayout.size();
		pipelineStateDescription.InputLayout.pInputElementDescs = m_ElementLayout.data();
		pipelineStateDescription.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// Vertex shader
		auto ite = m_Shaders.find(DX12ShaderCompiler::VertexShader);
		if (ite != m_Shaders.end())
		{
			pipelineStateDescription.VS.BytecodeLength = ite->second->GetBufferSize();
			pipelineStateDescription.VS.pShaderBytecode = ite->second->GetBufferPointer();
		}

		// Pixel shader
		ite = m_Shaders.find(DX12ShaderCompiler::PixelShader);
		if (ite != m_Shaders.end())
		{
			pipelineStateDescription.PS.BytecodeLength = m_Shaders[DX12ShaderCompiler::PixelShader]->GetBufferSize();
			pipelineStateDescription.PS.pShaderBytecode = m_Shaders[DX12ShaderCompiler::PixelShader]->GetBufferPointer();
		}

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

		// BlendState
		pipelineStateDescription.BlendState.AlphaToCoverageEnable = FALSE;
		pipelineStateDescription.BlendState.IndependentBlendEnable = TRUE;

		// NumRenderTargets
		pipelineStateDescription.NumRenderTargets = 0;

		for (uint32_t i = 0; i < formats.size() - 1; i++)
		{
			if (formats[i] == DXGI_FORMAT_UNKNOWN)
			{
				break;
			}

			pipelineStateDescription.NumRenderTargets++;
			pipelineStateDescription.RTVFormats[i] = formats[i];

			// RenderTarget BlendState
			pipelineStateDescription.BlendState.RenderTarget[i].BlendEnable = Utils::IsBlendValid(formats[i]) ? TRUE : FALSE;
			pipelineStateDescription.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			pipelineStateDescription.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			pipelineStateDescription.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			pipelineStateDescription.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_INV_DEST_ALPHA;
			pipelineStateDescription.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ONE;
			pipelineStateDescription.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			pipelineStateDescription.BlendState.RenderTarget[i].LogicOpEnable = FALSE;
			pipelineStateDescription.BlendState.RenderTarget[i].LogicOp = D3D12_LOGIC_OP_NOOP;
			pipelineStateDescription.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		}

		// DSVFormat
		pipelineStateDescription.DSVFormat = formats.back();

		// DepthStencilState
		pipelineStateDescription.DepthStencilState.DepthEnable = formats.back() != DXGI_FORMAT_UNKNOWN;
		pipelineStateDescription.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		pipelineStateDescription.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
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

		ID3D12PipelineState* pipeline = nullptr;
		HRESULT hr = DirectX12API::Get()->GetDevice()->CreateGraphicsPipelineState(&pipelineStateDescription, IID_PPV_ARGS(&pipeline));
		CHECK_DX12_HRESULT(hr, "Failed to create D3D12 Graphics Pipeline State Object");

		m_PipelineStates.insert(std::pair(formats, pipeline));
	
		return pipeline;
	}

	void DirectX12Shader::Bind()
	{
		LD_PROFILE_RENDERER_FUNCTION();

		// TODO:
		// if (m_CurreentFrame != Application::Get().GetCurrentFrame()
		// {
		// 	  m_CurrentFrame = Application::Get().GetCurrentFrame();
		//    m_CurrentShaderResource = 0;
		// }
		m_CurrentShaderResource = 0;

		ID3D12PipelineState* state = nullptr;
		auto ite = m_PipelineStates.find(DirectX12FrameBuffer::s_CurrentBoundFormats);
		if (ite == m_PipelineStates.end())
		{
			state = CreatePSOforFrameBufferFormats(DirectX12FrameBuffer::s_CurrentBoundFormats);
		}
		else
		{
			state = ite->second;
		}

		auto& commandList = DirectX12API::Get()->GetCommandList();

		commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->SetGraphicsRootSignature(s_RootSignature);
		commandList->SetPipelineState(state);
	}

	const std::string& DirectX12Shader::GetName() const
	{
		return m_Name;
	}

	void DirectX12Shader::SetViewProjectionMatrix(const DirectX::XMFLOAT4X4& matrix)
	{
		DirectX12API::Get()->GetCommandList()->SetGraphicsRoot32BitConstants(0, 16, &matrix, 0);
	}

	void DirectX12Shader::SetModelMatrix(const DirectX::XMFLOAT4X4& matrix)
	{
		DirectX12API::Get()->GetCommandList()->SetGraphicsRoot32BitConstants(0, 16, &matrix, 16);
	}

	void DirectX12Shader::UploadMaterialData(void* data)
	{
		if (m_CurrentShaderResource > (m_ShaderResources.size() - 1))
		{
			m_ShaderResources.emplace_back().Init(m_MaterialLayout.GetStride());
		}

		m_ShaderResources[m_CurrentShaderResource].FillBufferData(data, 0, m_MaterialLayout.GetStride());

		DirectX12API::Get()->GetCommandList()->SetGraphicsRootConstantBufferView(1, m_ShaderResources[m_CurrentShaderResource].GetBuffer()->GetGPUVirtualAddress());
		m_CurrentShaderResource++;
	}

	DX12ShaderCompiler::ShaderKind ShaderKindFromString(const std::string& kind)
	{
		if (kind == "vertex")
		{
			return DX12ShaderCompiler::VertexShader;
		}
		else if (kind == "pixel")
		{
			return DX12ShaderCompiler::PixelShader;
		}

		LD_CORE_ASSERT(false, "Syntax Error: Unknown Shader Kind '{0}'", kind);
		return DX12ShaderCompiler::Unknonw;
	}

	void DirectX12Shader::ReadFile(const std::filesystem::path& file, std::string& output)
	{
		LD_PROFILE_FUNCTION();

		std::ifstream srcFile(file, std::ios::in | std::ios::binary);
		if (srcFile)
		{
			size_t size = std::filesystem::file_size(file);
			output.resize(size);
			srcFile.seekg(0, std::ios::beg);
			srcFile.read(&output[0], size);
		}
		else
		{
			LD_CORE_ASSERT(false, "Failed to open file: {0}", file.string());
		}

	}

	void DirectX12Shader::ParseShaders(const std::string& source, std::unordered_map<DX12ShaderCompiler::ShaderKind, std::string>& shadersSrcs)
	{
		LD_PROFILE_FUNCTION();

		const char* typeToken = "#kind";
		size_t typeTokenLenght = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			LD_CORE_ASSERT(eol != std::string::npos, "Syntax Error: Shader declared but not defined!");
			size_t begin = pos + typeTokenLenght + 1; // Move to "#type "<-
			std::string kind = source.substr(begin, eol - begin);

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			LD_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax Error: Shader declared but not defined!");

			pos = source.find(typeToken, nextLinePos);

			shadersSrcs[ShaderKindFromString(kind)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}
	}

	void DirectX12Shader::CompileShaders(const std::unordered_map<DX12ShaderCompiler::ShaderKind, std::string> shadersSources, std::unordered_map<DX12ShaderCompiler::ShaderKind, IDxcBlob*>& output)
	{
		LD_PROFILE_FUNCTION();

		for (auto& [shaderKind, shaderSrc] : shadersSources)
		{
			DxcBuffer src = {};
			src.Ptr = shaderSrc.c_str();
			src.Size = shaderSrc.size();
			src.Encoding = DXC_CP_UTF8;

			bool result = DX12ShaderCompiler::Compile(src, shaderKind, output[shaderKind]);
			LD_CORE_ASSERT(result, "Failed to compile Shader");
		}
	}

	const BufferLayout& DirectX12Shader::GetVertexBufferLayout()
	{
		return m_VertexBufferLayout;
	}

	const BufferLayout& DirectX12Shader::GetMaterialLayout()
	{
		return m_MaterialLayout;
	}

	bool DirectX12Shader::CreateRootSignature()
	{
		LD_PROFILE_FUNCTION();

		/*
		*	Root Signature Layout (CPU)
		*	- 0		float4x4 "ViewProjection", float4x4 "Model"
		*	- 1		CBV "Material Data"
		*	- 2		descriptor table "Textures"
		*	
		* 
		*	Root Signature Layoutr (GPU)
		*	- b0	float4x4 "ViewProjection", float4x4 "Model"
		*	- b1	CBV
		*	- t0... Texture2D<float4> "Textures"
		*	- s0	Sampler for "Textures"
		*/

		D3D12_ROOT_PARAMETER1 cameraCBV = {};
		cameraCBV.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		cameraCBV.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		cameraCBV.Descriptor.ShaderRegister = 0;
		cameraCBV.Descriptor.RegisterSpace = 0;

		D3D12_ROOT_PARAMETER1 materialCBV = {};
		materialCBV.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		materialCBV.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		materialCBV.Descriptor.ShaderRegister = 1;
		materialCBV.Descriptor.RegisterSpace = 0;

		D3D12_DESCRIPTOR_RANGE1 srvRange = {};
		srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		srvRange.NumDescriptors = UINT_MAX;
		srvRange.BaseShaderRegister = 0;
		srvRange.RegisterSpace = 0;
		srvRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
		srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		D3D12_ROOT_PARAMETER1 textures = {};
		textures.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		textures.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		textures.DescriptorTable.NumDescriptorRanges = 1;
		textures.DescriptorTable.pDescriptorRanges = &srvRange;

#if 0
		D3D12_DESCRIPTOR_RANGE1 samplerRange = {};
		samplerRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		samplerRange.NumDescriptors = UINT_MAX;
		samplerRange.BaseShaderRegister = 0;
		samplerRange.RegisterSpace = 0;
		samplerRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		D3D12_ROOT_PARAMETER1 samplers = {};
		samplers.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		samplers.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		samplers.DescriptorTable.NumDescriptorRanges = 1;
		samplers.DescriptorTable.pDescriptorRanges = &samplerRange;
#endif

		D3D12_ROOT_PARAMETER1 parameters[] = { cameraCBV, materialCBV, textures };

		D3D12_STATIC_SAMPLER_DESC staticSampler = {};
		staticSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSampler.MipLODBias = 0.0f;
		staticSampler.MaxAnisotropy = 16;
		staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		staticSampler.MinLOD = 0.0f;
		staticSampler.MaxLOD = 0.0f;
		staticSampler.ShaderRegister = 0;
		staticSampler.RegisterSpace = 0;
		staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = {};
		rootSigDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		rootSigDesc.Desc_1_1.NumParameters = sizeof(parameters) / sizeof(D3D12_ROOT_PARAMETER);
		rootSigDesc.Desc_1_1.pParameters = parameters;
		rootSigDesc.Desc_1_1.NumStaticSamplers = 1;
		rootSigDesc.Desc_1_1.pStaticSamplers = &staticSampler;
		rootSigDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ID3DBlob* rootSigBlob = nullptr;
		ID3DBlob* rootSigErrors = nullptr;
		HRESULT hr = D3D12SerializeVersionedRootSignature(&rootSigDesc, &rootSigBlob, &rootSigErrors);
		CHECK_DX12_HRESULT(hr, "Failed to serialize Root Signature")
		{
			LD_CORE_TRACE("D3D12 Root Signature Serialization Errors: {0}", (char*)rootSigErrors->GetBufferPointer());
			CHECK_AND_RELEASE_COMPTR(rootSigErrors);
			return false;
		}

		hr = DirectX12API::Get()->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&s_RootSignature));
		CHECK_AND_RELEASE_COMPTR(rootSigBlob);
		CHECK_DX12_HRESULT(hr, "Failed to create D3D12 Root Signature")
		{
			return false;
		}

		return true;
	}

	void DirectX12Shader::DestroyRootSignature()
	{
		LD_PROFILE_FUNCTION();

		CHECK_AND_RELEASE_COMPTR(s_RootSignature);
	}

}