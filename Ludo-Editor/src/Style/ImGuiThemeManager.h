#pragma once

#include <unordered_map>

#include <LudoNarrative.h>
#include <imgui/imgui.h>

#include "Style/ImGuiTheme.h"

namespace Ludo {

	class ImGuiThemeManager
	{
	public:
		static void AddTheme(const std::string& name, const ImGuiTheme& theme);
		static const ImGuiTheme& GetTheme(const std::string& name);

		static void SetCurrentTheme(const std::string& themeName);
		static const ImGuiTheme& GetTheme();

	private:
		static ImGuiTheme s_CurrentTheme;
		static std::map<std::string, ImGuiTheme> s_Themes;
	};

}