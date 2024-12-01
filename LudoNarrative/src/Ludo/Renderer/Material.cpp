#include "ldpch.h"
#include "material.h"

namespace Ludo {

	void Material::SetMaterialItemData(const std::string& name, void* data)
	{
		auto& item = m_Items.at(name);

		uint8_t* dest = m_Buffer;
		dest += item.Offset;

		memcpy(dest, data, item.Size);
	}

	Material::Material(std::shared_ptr<Shader> shader)
		: m_Shader(shader)
	{
		m_Buffer = (uint8_t*)std::malloc(shader->GetMaterialLayout().GetStride());
		LD_CORE_ASSERT(m_Buffer, "Failed to create buffer for material");
		memset(m_Buffer, 0, shader->GetMaterialLayout().GetStride());

		for (auto& item : m_Shader->GetMaterialLayout())
		{
			m_Items.insert(std::pair<std::string, MaterialItemData>(item.Name, { item.Size, item.Offset }));
		}
	}

	Material::~Material()
	{
		std::free(m_Buffer);
	}

}