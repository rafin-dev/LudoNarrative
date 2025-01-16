#include "ImGuiThemeManager.h"

namespace Ludo {

	ImGuiTheme ImGuiThemeManager::s_CurrentTheme;
	std::map<std::string, ImGuiTheme> ImGuiThemeManager::s_Themes;

	void ImGuiThemeManager::AddTheme(const std::string& name, const ImGuiTheme& theme)
	{
		LD_ASSERT(!name.empty(), "Atempt to register theme with empty name");
		LD_ASSERT(s_Themes.find(name) == s_Themes.end(), "Theme '{0}' alredy exists", name);

		s_Themes.insert(std::pair(name, theme));
	}

	const ImGuiTheme& ImGuiThemeManager::GetTheme(const std::string& name)
	{
		auto ite = s_Themes.find(name);
		LD_ASSERT(ite != s_Themes.end(), "Theme '{0}' does not exist");
		return ite->second;
	}

	void ImGuiThemeManager::SetCurrentTheme(const std::string& themeName)
	{
		auto ite = s_Themes.find(themeName);
		LD_ASSERT(ite != s_Themes.end(), "ImGuiTheme '{0}' does not exist", themeName);

		s_CurrentTheme = ite->second;

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();

		style.Colors[ImGuiCol_WindowBg] = s_CurrentTheme.Colors.WindowBackground;

		style.Colors[ImGuiCol_Header] = s_CurrentTheme.Colors.Header;
		style.Colors[ImGuiCol_HeaderHovered] = s_CurrentTheme.Colors.HeaderHovered;
		style.Colors[ImGuiCol_Header] = s_CurrentTheme.Colors.HeaderHovered;

		style.Colors[ImGuiCol_Button] = s_CurrentTheme.Colors.Button;
		style.Colors[ImGuiCol_ButtonHovered] = s_CurrentTheme.Colors.ButtonHovered;
		style.Colors[ImGuiCol_ButtonActive] = s_CurrentTheme.Colors.ButtonActive;

		style.Colors[ImGuiCol_FrameBg] = s_CurrentTheme.Colors.FrameBackground;
		style.Colors[ImGuiCol_FrameBgHovered] = s_CurrentTheme.Colors.FrameBackgroundHovered;
		style.Colors[ImGuiCol_FrameBgActive] = s_CurrentTheme.Colors.FrameBackgroundActive;

		style.Colors[ImGuiCol_Tab] = s_CurrentTheme.Colors.Tab;
		style.Colors[ImGuiCol_TabHovered] = s_CurrentTheme.Colors.TabHovered;
		style.Colors[ImGuiCol_TabActive] = s_CurrentTheme.Colors.TabActive;
		style.Colors[ImGuiCol_TabUnfocused] = s_CurrentTheme.Colors.TabUnfocused;
		style.Colors[ImGuiCol_TabUnfocusedActive] = s_CurrentTheme.Colors.TabUnfocusedActive;

		style.Colors[ImGuiCol_TitleBg] = s_CurrentTheme.Colors.TitleBackground;
		style.Colors[ImGuiCol_TitleBgActive] = s_CurrentTheme.Colors.TitleBackgroundActive;
		style.Colors[ImGuiCol_TitleBgCollapsed] = s_CurrentTheme.Colors.TitleBackgroundCollapsed;

		ImGuiFontManager::Get()->SetDefaultFont(s_CurrentTheme.Font.GetFont());
	}

	const ImGuiTheme& ImGuiThemeManager::GetTheme()
	{
		return s_CurrentTheme;
	}

}