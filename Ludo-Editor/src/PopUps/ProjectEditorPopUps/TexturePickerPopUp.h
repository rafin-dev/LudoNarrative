#pragma once

#include "PopUps/PopUpWindow.h"

#include "LudoNarrative.h"

namespace Ludo {

	class TexturePickerPopUp : public PopUpWindow
	{
	public:
		TexturePickerPopUp(Entity entity);

		void OnImGuiRender() override;

		bool ShouldClose() override { return m_ShouldClose; }

	private:
		Entity m_Entity;

		bool m_ShouldClose = false;
	};

}