#pragma once

#include "Ludo/Renderer/Shader.h"

#include <unordered_map>
#include <string>

namespace Ludo {

	class Material
	{
	public:
		static Ref<Material> Create(Ref<Shader> shader)
		{
			return Ref<Material>(new Material(shader));
		}

		void SetMaterialItemData(const std::string& name, void* data);

		void UploadMaterialData()
		{
			m_Shader->UploadMaterialData(m_Buffer);
		}

		~Material();

		Ref<Shader> GetShader() { return m_Shader; }

	private:
		Material(Ref<Shader> shader);

		Ref<Shader> m_Shader;

		struct MaterialItemData
		{
			uint32_t Size = 0;
			uint32_t Offset = 0;
		};
		std::unordered_map<std::string, MaterialItemData> m_Items;

		uint8_t* m_Buffer = nullptr;
	};

}