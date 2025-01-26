#pragma once

#include "Ludo/Core/Core.h"
#include "Ludo/Renderer/Texture.h"
#include "Ludo/Renderer/SubTexture2D.h"

#include <imgui/imgui.h>

namespace Ludo {

	class ImGuiTexture
	{
	public:
		virtual ~ImGuiTexture() = default;

		virtual ImTextureID GetImTextureID() = 0;

		virtual Ref<SubTexture2D> GetSubTexture() = 0;
		virtual Ref<Texture2D> GetTexture() = 0;

		static Ref<ImGuiTexture> Create(const Ref<Texture2D>& texture);
		static Ref<ImGuiTexture> Create(const Ref<SubTexture2D>& texture);
	};

}