#pragma once

#include "Ludo/Core/Core.h"

#include <string>
#include <filesystem>

namespace Ludo {

	class Project
	{
	public:
		static void New(const std::filesystem::path& path, const std::string& name);
		static void Load(const std::filesystem::path& path);
		static void Save();

		static const std::string& GetName() { return s_ActiveProject->m_Name; }
		static const std::filesystem::path& GetRootDirectory() { return s_ActiveProject->m_AssetDirectory.parent_path(); }
		static const std::filesystem::path& GetAssetDirectory() { return s_ActiveProject->m_AssetDirectory; }
		static const std::filesystem::path& GetMetadataDirectory() { return s_ActiveProject->m_MetadataDirectory; }

	private:

		std::string m_Name;
		std::filesystem::path m_AssetDirectory;
		std::filesystem::path m_MetadataDirectory;

		inline static Ref<Project> s_ActiveProject;
		inline static std::filesystem::path s_ActiveProjectFilePath;

		friend class ProjectSerializer;
	};

}