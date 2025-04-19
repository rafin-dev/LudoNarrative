#include "TexturePickerPopUp.h"

namespace Ludo {

	TexturePickerPopUp::TexturePickerPopUp(Entity entity)
	{
		m_Entity = entity;
	}

	void TexturePickerPopUp::OnImGuiRender()
	{
		ImGui::Begin("Texture Picker");

		if (!ImGui::IsWindowFocused())
		{
			m_ShouldClose = true;
		}

		UUID uuid = 0;

		for (auto& ite : AssetImporter::s_MetadataList)
		{
			if (ite.second.Type != AssetType::Texture2D)
			{
				continue;
			}

			if (ImGui::Button(ite.second.RawFilePath.stem().string().c_str()))
			{
				uuid = ite.second.AssetUUID;
			}
		}

		ImGui::End();

		if (uuid)
		{
			auto& sprite = m_Entity.GetComponent<SpriteRendererComponent>();

			AssetHandle tx = AssetManager::LoadAsset(uuid);
			sprite.Texture = tx;
			m_ShouldClose = true;
		}
	}

}