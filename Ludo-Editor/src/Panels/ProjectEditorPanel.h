#pragma once

#include "PanelBase.h"

#include "ProjectEditorViews/ViewBase.h"

#include <unordered_map>

namespace Ludo {

	class ProjectEditorPanel : public PanelBase
	{
	public:
		ProjectEditorPanel();
		~ProjectEditorPanel();

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate(TimeStep ts) override;
		void OnImGui() override;

		void OnEvent(Event& e) override;
		bool OnPanelChange(PanelCodes newPanel) override;

	private:
		enum class Views
		{
			EntityProperties = 0,
			SceneHierarchy,
			ContentBrowser,
			SceneViewer,
			AssetManagerStatus,
			AssetImporterStatus
		};

		std::unordered_map<Views, Ref<ProjectEditorViewBase>> m_Views;

	};

}