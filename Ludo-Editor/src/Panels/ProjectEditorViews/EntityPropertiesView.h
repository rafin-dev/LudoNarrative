#pragma once

#include "ViewBase.h"

#include "misc/OpenAndSelectedsManager.h"

#include <unordered_map>

namespace Ludo {

	class EntityPropertiesView : public ProjectEditorViewBase
	{
	public:
		EntityPropertiesView(const Ref<OpenAndSelectedsManager>& oas);
		~EntityPropertiesView() override;

		void OnImGuiRender() override;

		template<typename T, bool Removable = true>
		void RenderComponentPropertiesIfExists(const std::string& label);
		
	private:
		void OnRenderEntityTagAndComponentOptions();

		void OnRenderTransformComponent();
		void OnRenderSpriteRendererComponent();
		void OnRenderCircleRendererComponent();
		void OnRenderScriptComponent();
		void OnRenderCameraComponent();
		void OnRenderRigidbody2DComponent();
		void OnRenderBoxCollider2DComponent();
		void OnRenderCircleCollider2DComponent();

		Ref<OpenAndSelectedsManager> m_OpenAndSelected;

		std::unordered_map<size_t, std::function<void()>> m_PropertieRendereres;
	};

}