#pragma once
#include <LudoNarrative.h>

#include "Panels/PanelBase.h"
#include "PopUps/PopUpWindow.h"

#include <unordered_map>
#include <list>

namespace Ludo {

	class EditorApplication : public Application
	{
	public:
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(TimeStep time) override;
		void OnImGuiRender() override;
		void OnEvent(Event& event) override;

		static void SetPanel(PanelCodes newPanel);
		
		template<typename T, typename... Args>
		static void CreatePopUp(Args&&... args)
		{
			static_assert(std::is_base_of_v<PopUpWindow, T>);

			Ref<PopUpWindow> popup = CreateRef<T>(std::forward<Args>(args)...);
			((EditorApplication&)Application::Get()).m_PopUps.push_back(popup);
		}

	private:
		void SetPanelImpl(PanelCodes newPanel);

		PanelCodes m_CurrentPanel = PanelCodes::ProjectSelector;

		std::map<PanelCodes, Scope<PanelBase>> m_Panels;
		std::list<Ref<PopUpWindow>> m_PopUps;
	};
	
}