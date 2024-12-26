#include "ldpch.h"
#include "Shader.h"

#include "Ludo/Core/Core.h"
#include "Renderer.h"

#ifdef LD_PLATFORM_WINDOWS
    #include "Platform/DirectX11/DirectX11Shader.h"
    #include "Platform/DirectX12/DirectX12Shader.h"
#endif 

namespace Ludo {

    Ref<Shader> Shader::Create(
        const std::string& name,
        void* vertexShaderBuffer, size_t vertexShaderSize,
        void* pixelShaderBuffer, size_t pixelShaderSize,
        const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None: LD_CORE_ASSERT(false, "RenderAPI::None is currently not supported") return nullptr;
            case RendererAPI::API::DirectX11: return CreateRef<DirectX11Shader>(name, vertexShaderBuffer, vertexShaderSize, pixelShaderBuffer, pixelShaderSize, vertexLayout, materialDataLayout);
            case RendererAPI::API::DirectX12: return CreateRef<DirectX12Shader>(name, vertexShaderBuffer, vertexShaderSize, pixelShaderBuffer, pixelShaderSize, vertexLayout, materialDataLayout);
        }

        LD_CORE_ASSERT(false, "Unknown RenderAPI specified, please provide a existing RenderAPI!");
        return nullptr;
    }

    Ref<Shader> Shader::Create(
        const std::string& name, const std::filesystem::path& shaderSrcPath, const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None: LD_CORE_ASSERT(false, "RenderAPI::None is currently not supported") return nullptr;
            case RendererAPI::API::DirectX11: return CreateRef<DirectX11Shader>(name, shaderSrcPath, vertexLayout, materialDataLayout);
            case RendererAPI::API::DirectX12: return CreateRef<DirectX12Shader>(name, shaderSrcPath, vertexLayout, materialDataLayout);
        }

        LD_CORE_ASSERT(false, "Unknown RenderAPI specified, please provide a existing RenderAPI!");
        return nullptr;
    }

    // ========== Shader Library ===========
    void ShaderLibrary::Add(const Ref<Shader>& shader)
    {
        auto& name = shader->GetName();
        LD_CORE_ASSERT(!Exists(name), "Shader '{0}' already exists in Shader Library", name);
        m_Shaders[name] = shader;
    }

    Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::filesystem::path& shaderSrcPath, const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout)
    {
        Ref<Shader> shader = Shader::Create(name, shaderSrcPath, vertexLayout, materialDataLayout);
        Add(shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Load(const std::string& name,
        void* vertexShaderBuffer, size_t vertexShaderSize,
        void* pixelShaderBuffer, size_t pixelShaderSize,
        const BufferLayout& vertexLayout, const BufferLayout& materialDataLayout)
    {
        Ref<Shader> shader = Shader::Create(name, vertexShaderBuffer, vertexShaderSize, pixelShaderBuffer, pixelShaderSize, vertexLayout, materialDataLayout);
        Add(shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Get(const std::string& name)
    {
        LD_CORE_ASSERT(Exists(name), "Failed to find Shader '{0}' in Shader Library", name);
        return m_Shaders[name];
    }

    bool ShaderLibrary::Exists(const std::string& name) const
    {
        return m_Shaders.find(name) != m_Shaders.end();
    }

}