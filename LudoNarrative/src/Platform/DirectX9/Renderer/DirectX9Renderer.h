#pragma once

#include "Ludo/Renderer/InternalRenderer.h"

namespace Ludo {

	class DirectX9Renderer : public InternalRenderer
	{
	public:
		void Init() override;
		~DirectX9Renderer() override;

		void BeginScene() override;
		void EndScene() override;

	private:

		LPDIRECT3D9 Interface;
		LPDIRECT3DDEVICE9 Device;
	};

}