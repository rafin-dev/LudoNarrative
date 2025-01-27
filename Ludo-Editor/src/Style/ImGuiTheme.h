#pragma once

#include "Style/ImGuiFontManager.h"
#include <imgui.h>

namespace Ludo{

	struct ImGuiThemeColors
	{
		// headers
		ImVec4 WindowBackground;
		ImVec4 Header;
		ImVec4 HeaderHovered;
		ImVec4 HeaderActive;

		// Buttons
		ImVec4 Button;
		ImVec4 ButtonHovered;
		ImVec4 ButtonActive;

		// Frame Background
		ImVec4 FrameBackground;
		ImVec4 FrameBackgroundHovered;
		ImVec4 FrameBackgroundActive;

		// Tabs
		ImVec4 Tab;
		ImVec4 TabHovered;
		ImVec4 TabActive;
		ImVec4 TabUnfocused;
		ImVec4 TabUnfocusedActive;

		// Title Background
		ImVec4 TitleBackground;
		ImVec4 TitleBackgroundActive;
		ImVec4 TitleBackgroundCollapsed;
	};

	struct ImGuiTheme
	{
		ImGuiThemeColors Colors;
		// TODO: Add support for multiple fonts in a single theme
		ImGuiFontManager::FontInstance Font = ImGuiFontManager::Get()->GetDefaultFont();
	};

}