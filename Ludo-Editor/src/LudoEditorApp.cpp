#include <LudoNarrative.h>
#include <Ludo/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Ludo {

	class LudoEditor : public Application
	{
	public:
		LudoEditor()
			: Application("Ludo Editor")
		{
			PushLayer(new EditorLayer());
		}

		~LudoEditor()
		{

		}
	};

	Application* CreateApplication()
	{
		return new LudoEditor();
	}
}