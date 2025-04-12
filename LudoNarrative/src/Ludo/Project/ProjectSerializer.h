#pragma once

#include "Ludo/Core/Core.h"
#include "Ludo/Project/Project.h"

namespace Ludo {

	class ProjectSerializer
	{
	public:
		ProjectSerializer(const Ref<Project> project);

		void Serialize(const std::filesystem::path& path);
		bool Deserialize(const std::filesystem::path& path);

	private:
		Ref<Project> m_Project;
	};
	
}