#include "ldpch.h"
#include "ProjectSerializer.h"

#include <yaml-cpp/yaml.h>

namespace Ludo {

	ProjectSerializer::ProjectSerializer(const Ref<Project> project)
		: m_Project(project)
	{
	}

	void ProjectSerializer::Serialize(const std::filesystem::path& path)
	{
		LD_CORE_ASSERT(path.extension() == ".ldProject");

		YAML::Emitter out;

		out << YAML::BeginMap;

		out << YAML::Key << "Project" << YAML::Value << m_Project->m_Name;
		out << YAML::Key << "AssetDirectory" << YAML::Value << m_Project->m_AssetDirectory.string();

		out << YAML::EndMap;

		std::ofstream stream(path);

		stream << out.c_str();
	}

	bool ProjectSerializer::Deserialize(const std::filesystem::path& path)
	{
		LD_CORE_ASSERT(path.extension() == ".ldProject");

		if (!std::filesystem::exists(path))
		{
			LD_CORE_ERROR("Failed to load Project at '{0}': Project file not found");
			return false;
		}

		std::ifstream stream(path);
		std::stringstream sstream;
		sstream << stream.rdbuf();

		YAML::Node data = YAML::Load(sstream);
		if (!data["Project"])
		{
			LD_CORE_ERROR("Failed to load Project at '{0}': File did not contain project data");
			return false;
		}

		m_Project->m_Name = data["Project"].as<std::string>();
		m_Project->m_AssetDirectory = data["AssetDirectory"].as<std::string>();

		return true;
	}

}