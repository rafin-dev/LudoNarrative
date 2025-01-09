#pragma once

#include "Ludo/Core/Core.h"
#include "Ludo/Renderer/Texture.h"

#include <imgui/imgui.h>

namespace Ludo {

	class ImGuiTexture
	{
	public:
		virtual ~ImGuiTexture() = default;

		virtual ImTextureID GetImTextureID() = 0;

		static Ref<ImGuiTexture> Create(const Ref<Texture2D>& texture);
	};

}