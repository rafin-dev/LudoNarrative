#pragma once

#include "ViewBase.h"

#include "LudoNarrative.h"

#include <filesystem>

namespace Ludo {

	class ContentBrowserView : public ProjectEditorViewBase
	{
	public:
		ContentBrowserView();
		~ContentBrowserView() override;

		void OnImGuiRender() override;

	private:
		std::filesystem::path m_AssetDirectory;
		std::filesystem::path m_CurrentDirectory;

		enum class EntryType
		{
			Directory,
			Scene,
			Texture2D
		};
		
		std::unordered_map<std::string, EntryType> m_Types;
		std::unordered_map<EntryType, Ref<Texture2D>> m_Icons;
	};

}