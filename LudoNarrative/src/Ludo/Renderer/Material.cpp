#include "ldpch.h"
#include "Material.h"

namespace Ludo {

	Material::Material(std::shared_ptr<Shader> shader)
		: m_Shader(shader)
	{
		m_Shader->AddEntry();

		for (auto& element : m_Shader->GetMaterialLayout())
		{
			m_MaterialDataInformation.insert(std::pair(element.Name, std::pair(element.Size, element.Offset)));
		}

		m_MaterialData = (uint8_t*)malloc(m_MaterialLayout.GetStride());
	}

	Material::~Material()
	{
		LD_CORE_TRACE("Destroyed material");
		m_Shader->RemoveEntry();
		free(m_MaterialData);
		m_MaterialData = nullptr;
	}

	void Material::UploadData()
	{
		m_Shader->UploadMaterialDataBuffer(m_MaterialData);
	}

	void Material::SetMaterialData(const std::string& elementName, void* data)
	{
		auto ite = m_MaterialDataInformation.find(elementName);

#ifdef LUDO_DEBUG
		if (ite == m_MaterialDataInformation.end())
		{
			LD_CORE_ERROR("Material Element {0} not found!", elementName);
			return;
		}
#endif

		auto& [size, offset] = ite->second;

		uint8_t* dest = m_MaterialData;
		dest += offset;

		memcpy(dest, data, size);
	}

}