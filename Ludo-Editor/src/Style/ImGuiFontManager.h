#pragma once

#include <unordered_map>
#include <filesystem>

#include <LudoNarrative.h>
#include <imgui/imgui.h>

namespace Ludo {
	
	/// <summary>
	/// Any font type set to empty will use Regular as default
	/// Regular cannot be empty
	/// </summary>
	struct CustomFontDescription
	{
		float SizeInPixels = 15.0f;
		std::filesystem::path RegularFont;
		std::filesystem::path BoldFont;
		std::filesystem::path ItalicFont;
	};

	class ImGuiFontManager
	{
	public:
		static ImGuiFontManager* Get();

		struct Font
		{
		public:
			ImFont* RegularFont = nullptr;
			ImFont* BoldFont = nullptr;
			ImFont* ItalicFont = nullptr;

			Font(ImFont* regular, ImFont* bold, ImFont* italic)
				: RegularFont(regular), BoldFont(bold), ItalicFont(italic) 
			{}
			Font() = default;
		};

		class FontInstance
		{
		public:
			FontInstance(const Font* font) : m_Font(font) {}
			~FontInstance() { if (m_StyleOverridden) { PopStyle(); } }

			void PushRegularStyle() { if (m_StyleOverridden) { PopStyle(); } ImGui::PushFont(m_Font->RegularFont); }
			void PushBoldStyle() { if (m_StyleOverridden) { PopStyle(); } ImGui::PushFont(m_Font->BoldFont); }
			void PushItalicStyle() { if (m_StyleOverridden) { PopStyle(); } ImGui::PushFont(m_Font->ItalicFont); }
			
			void PopStyle() { ImGui::PopFont(); m_StyleOverridden = false; }

			const Font* GetFont() { return m_Font; }

		private:
			const Font* m_Font;
			bool m_StyleOverridden = false;
		};

		FontInstance AddFont(const std::string& name,const CustomFontDescription& fontDesc);
		FontInstance GetFont(const std::string& name);

		void PushDefaultFontBoldStyle();
		void PushDefaultFontItalicStyle();
		void PopDefaultFontStyle();

		void SetDefaultFont(const Font* font);
		const FontInstance GetDefaultFont();

		void Init();

	private:
		Font m_DefaultFont;
		std::unordered_map<std::string, Font> m_Fonts;
	};

}