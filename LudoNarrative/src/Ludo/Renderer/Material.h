#pragma once

#include "Ludo/Log.h"
#include "Shader.h"

namespace Ludo {

	class Material
	{
	public:
		Material(std::shared_ptr<Shader> shader);
		~Material();
	
		void UploadData();

		void SetMaterialData(const std::string& elementName, void* data);

		std::shared_ptr<Shader> GetShader() { return m_Shader; }

		const BufferLayout& GetLayout() { return m_Shader->GetMaterialLayout(); }

	private:
		std::shared_ptr<Shader> m_Shader;
		BufferLayout m_MaterialLayout = {};

		uint8_t* m_MaterialData = nullptr;

		// Name -> { Size, Offset }
		std::unordered_map<std::string, std::pair<size_t, size_t>> m_MaterialDataInformation = {};
	};

}