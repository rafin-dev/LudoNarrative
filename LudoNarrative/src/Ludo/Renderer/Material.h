#pragma once

#include "Ludo/Renderer/Shader.h"

#include <unordered_map>
#include <string>

namespace Ludo {

	class Material
	{
	public:
		static std::shared_ptr<Material> Create(std::shared_ptr<Shader> shader)
		{
			return std::shared_ptr<Material>(new Material(shader));
		}

		void SetMaterialItemData(const std::string& name, void* data);

		void UploadMaterialData()
		{
			m_Shader->UploadMaterialData(m_Buffer);
		}

		~Material();

		std::shared_ptr<Shader> GetShader() { return m_Shader; }

	private:
		Material(std::shared_ptr<Shader> shader);

		std::shared_ptr<Shader> m_Shader;

		struct MaterialItemData
		{
			uint32_t Size = 0;
			uint32_t Offset = 0;
		};
		std::unordered_map<std::string, MaterialItemData> m_Items;

		uint8_t* m_Buffer = nullptr;
	};

}