#pragma once

#include "Ludo/Core/Core.h"
#include "Ludo/Scene/Scene.h"

#include <filesystem>

namespace Ludo {

	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void SerializeToYamlFile(const std::filesystem::path& filePath);
		void SerializeBinaryToFile(const std::filesystem::path& filePath);

		bool DeserializeFromYamlFile(const std::filesystem::path& filePath);
		bool DeserializeBinaryFromFile(const std::filesystem::path& filePath);

	private:
		Ref<Scene> m_Scene;
	};

}