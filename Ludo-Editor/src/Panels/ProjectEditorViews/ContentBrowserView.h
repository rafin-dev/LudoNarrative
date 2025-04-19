#pragma once

#include "ViewBase.h"

#include "LudoNarrative.h"

#include "misc/OpenAndSelectedsManager.h"

#include <filesystem>

namespace Ludo {

	class ContentBrowserView : public ProjectEditorViewBase
	{
	public:
		ContentBrowserView(const Ref<OpenAndSelectedsManager>& oas);
		~ContentBrowserView() override;

		void OnImGuiRender() override;

		bool OnEvent(Event& event);

	private:
		bool OnFileDropped(FileDroppedEvent& event);

		Ref<OpenAndSelectedsManager> m_OpenAndSelecteds;
		
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