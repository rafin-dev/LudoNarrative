#pragma once

#include "Ludo/Renderer/Renderer2D.h"

namespace Ludo {

	class DirectX12Renderer2D : public Renderer2D
	{
	public:
		bool Init() override;
		~DirectX12Renderer2D() override;

	private:
		void ShutDown();

		ID3D12RootSignature* m_RootSignature = nullptr;
		ID3D12PipelineState* m_PipelineStateObject = nullptr;
	};

}