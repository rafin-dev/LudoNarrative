#include "ldpch.h"
#include "Project.h"

#include "ProjectSerializer.h"

#include "LudoNarrative.h"

namespace Ludo {

	void Project::New(const std::filesystem::path& path, const std::string& name)
	{
		s_ActiveProject = CreateRef<Project>();
		s_ActiveProjectFilePath = path;

		s_ActiveProject->m_Name = name;
		s_ActiveProject->m_AssetDirectory = s_ActiveProjectFilePath.parent_path() / "Assets";
		s_ActiveProject->m_MetadataDirectory = s_ActiveProjectFilePath.parent_path() / ".LudoNarrative";

		std::filesystem::create_directories(s_ActiveProject->m_AssetDirectory);
		std::filesystem::create_directories(s_ActiveProject->m_MetadataDirectory);

		ProjectSerializer serializer(s_ActiveProject);
		serializer.Serialize(s_ActiveProjectFilePath);

		LD_CORE_INFO("Created Project: {0}", name);
	}

	void Project::Load(const std::filesystem::path& path)
	{
		Ref<Project> project = CreateRef<Project>();
		ProjectSerializer serializer(project);

		if (serializer.Deserialize(path))
		{
			s_ActiveProject = project;
			s_ActiveProjectFilePath = path;
		}

		std::string title = "Project: " + project->GetName();
		Application::Get().GetWindow().SetTitle(title);

		AssetImporter::LoadProjectAssetMetadataList();

		LD_CORE_INFO("Loaded Project: {0}", s_ActiveProject->GetName());
	}

	void Project::Save()
	{
		ProjectSerializer serializer(s_ActiveProject);
		serializer.Serialize(s_ActiveProjectFilePath);
	}

}