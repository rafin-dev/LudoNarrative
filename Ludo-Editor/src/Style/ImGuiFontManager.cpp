#include "ImGuiFontManager.h"

namespace Ludo {

    ImGuiFontManager* ImGuiFontManager::Get()
    {
        static ImGuiFontManager instance;
        return &instance;
    }

    ImGuiFontManager::FontInstance ImGuiFontManager::AddFont(const std::string& name, const CustomFontDescription& fontDesc)
    {
        LD_ASSERT(!name.empty(), "Atemp to register nameless Font");
        LD_ASSERT(m_Fonts.find(name) == m_Fonts.end(), "Font with name '{0}' alredy registered", name);
        LD_ASSERT(!fontDesc.RegularFont.empty(), "Atempt to create Font without Regular Style");

        auto& io = ImGui::GetIO();
        auto regularFont = io.Fonts->AddFontFromFileTTF(fontDesc.RegularFont.string().c_str(), fontDesc.SizeInPixels);
        auto boldFont = !fontDesc.BoldFont.empty() ? io.Fonts->AddFontFromFileTTF(fontDesc.BoldFont.string().c_str(), fontDesc.SizeInPixels) : regularFont;
        auto italicFont = !fontDesc.ItalicFont.empty() ? io.Fonts->AddFontFromFileTTF(fontDesc.ItalicFont.string().c_str(), fontDesc.SizeInPixels) : regularFont;

        auto font = &m_Fonts.insert(std::pair(name, Font(regularFont, boldFont, italicFont))).first->second;

        return FontInstance(font);
    }

    ImGuiFontManager::FontInstance ImGuiFontManager::GetFont(const std::string& name)
    {
        LD_ASSERT(!name.empty(), "Atempt to search for Font with empty name");

        auto ite = m_Fonts.find(name);

        LD_ASSERT(ite != m_Fonts.end(), "Font '{0}' does not exist", name);

        return FontInstance(&ite->second);
    }

    void ImGuiFontManager::PushDefaultFontBoldStyle()
    {
        ImGui::PushFont(m_DefaultFont.BoldFont);
    }

    void ImGuiFontManager::PushDefaultFontItalicStyle()
    {
        ImGui::PushFont(m_DefaultFont.ItalicFont);
    }

    void ImGuiFontManager::PopDefaultFontStyle()
    {
        ImGui::PopFont();
    }

    void ImGuiFontManager::SetDefaultFont(const Font* font)
    {
        m_DefaultFont = *font;

        auto& io = ImGui::GetIO();
        io.FontDefault = font->RegularFont;
    }

    const ImGuiFontManager::FontInstance ImGuiFontManager::GetDefaultFont()
    {
        return FontInstance(&m_DefaultFont);
    }

    void ImGuiFontManager::Init()
    {
        auto defaultImGuiFont = ImGui::GetFont();
        m_DefaultFont = Font(defaultImGuiFont, defaultImGuiFont, defaultImGuiFont);
    }

}