#include "ldpch.h"
#include "DX12ShaderCompiler.h"

#include "Platform/DirectX12/Utils/DX12Utils.h"

namespace Ludo {

    IDxcUtils* DX12ShaderCompiler::s_CompilerUtils = nullptr;
    IDxcCompiler3* DX12ShaderCompiler::s_Compiler = nullptr;
    IDxcIncludeHandler* DX12ShaderCompiler::s_IncludeHandler = nullptr;

    bool DX12ShaderCompiler::Init()
    {
        LD_PROFILE_FUNCTION();

        HRESULT hr = S_OK;

        hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&s_CompilerUtils));
        VALIDATE_DX12_HRESULT(hr, "Failed to create DirectX Compiler Utils Interface");

        hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&s_Compiler));
        VALIDATE_DX12_HRESULT(hr, "Failed to create DirectX Compiler Interface");

        hr = s_CompilerUtils->CreateDefaultIncludeHandler(&s_IncludeHandler);
        VALIDATE_DX12_HRESULT(hr, "Failed to create Default Include Handler");

        return true;
    }

    void DX12ShaderCompiler::ShutDown()
    {
        LD_PROFILE_FUNCTION();
        
        CHECK_AND_RELEASE_COMPTR(s_CompilerUtils);
        CHECK_AND_RELEASE_COMPTR(s_Compiler);
        CHECK_AND_RELEASE_COMPTR(s_IncludeHandler);
    }

    static LPCWSTR GetTargetFromShaderKind(const DX12ShaderCompiler::ShaderKind& kind)
    {
        switch (kind)
        {
        case DX12ShaderCompiler::PixelShader: return L"ps_6_0";
        case DX12ShaderCompiler::VertexShader: return L"vs_6_0";
        }

        LD_CORE_ASSERT(false, "Unknow Shader Kind");
        return nullptr;
    }

    bool DX12ShaderCompiler::Compile(const DxcBuffer& srcCode, ShaderKind shaderKind, IDxcBlob*& output)
    {
        LD_PROFILE_FUNCTION();

        std::vector<LPCWSTR> arguments;
        arguments.reserve(4);

        arguments.push_back(L"-E");
        arguments.push_back(L"main");

        arguments.push_back(L"-T");
        arguments.push_back(GetTargetFromShaderKind(shaderKind));

        arguments.push_back(L"-Qstrip_reflect");
        arguments.push_back(L"-Fre");
        arguments.push_back(L"refPath");

        HRESULT hr = S_OK;

        IDxcResult* result = nullptr;
        s_Compiler->Compile(&srcCode, arguments.data(), (UINT32)arguments.size(), s_IncludeHandler, IID_PPV_ARGS(&result));
        result->GetStatus(&hr);

        IDxcBlobUtf8* errors = nullptr;
        result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
        if (errors && errors->GetStringLength())
        {
            LD_CORE_WARN("Shader Compilation Errors/Warnings: {0}", (char*)errors->GetBufferPointer());
            errors->Release();
        }
        if (FAILED(hr))
        {
            LD_CORE_ERROR("Failed to compile Shader");
            return false;
        }

        hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&output), nullptr);
        CHECK_DX12_HRESULT(hr, "Failed to retrieve Shader Compilation Result");

        result->Release();

        return true;
    }

    bool DX12ShaderCompiler::CreateBlob(size_t size, IDxcBlob** output)
    {
        IDxcBlobEncoding* blobEnconding = nullptr;
        HRESULT hr = s_CompilerUtils->CreateBlob(nullptr, (UINT32)size, DXC_CP_ACP, &blobEnconding);
        if (FAILED(hr))
        {
            *output = nullptr;
            return false;
        }

        *output = blobEnconding;
        return true;
    }

}