#pragma once

#include "Ludo/Renderer/InternalRenderer.h"

namespace Ludo {

	class DirectX9Renderer : public InternalRenderer
	{
	public:
		void Init() override;
		~DirectX9Renderer() override;

		void Resize(unsigned int width, unsigned int height);

		void BeginScene() override;
		void EndScene() override;

	private:

		void ResetDevice();

		LPDIRECT3D9 Interface;
		LPDIRECT3DDEVICE9 Device;
		D3DPRESENT_PARAMETERS Parameters = {};
	};

}