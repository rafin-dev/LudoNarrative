#pragma once

#include <dxcapi.h>

namespace Ludo {

	class DX12ShaderCompiler
	{
	public:
		enum ShaderKind
		{
			Unknonw = -1,
			VertexShader = 0,
			PixelShader
		};

		static bool Init();
		static void ShutDown();

		static bool Compile(const DxcBuffer& srcCode, ShaderKind shaderKind, IDxcBlob*& output);

		static bool CreateBlob(size_t size, IDxcBlob** output);

	private:
		static IDxcUtils* s_CompilerUtils;
		static IDxcCompiler3* s_Compiler;
		static IDxcIncludeHandler* s_IncludeHandler;
	};

}