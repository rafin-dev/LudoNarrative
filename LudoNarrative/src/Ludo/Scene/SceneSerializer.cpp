#include "ldpch.h"
#include "SceneSerializer.h"

#include "Ludo/Scene/Entity.h"
#include "Ludo/Scene/Components.h"

#include <yaml-cpp/yaml.h>

#include <fstream>

namespace YAML {

	template<>
	struct convert<DirectX::XMFLOAT3>
	{
		static Node encode(const DirectX::XMFLOAT3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, DirectX::XMFLOAT3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<DirectX::XMFLOAT4>
	{
		static Node encode(const DirectX::XMFLOAT4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, DirectX::XMFLOAT4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
			{
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

}

namespace Ludo {

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT4& vec4)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec4.x << vec4.y << vec4.z << vec4.w << YAML::EndSeq;
		return out;
	}

	static YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT3& vec3)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec3.x << vec3.y << vec3.z << YAML::EndSeq;
		return out;
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << "1234567"; // TODO: Entity UUID

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent" << YAML::BeginMap; // TagComponent

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap; // TagComponent
		}
		else
		{
			LD_CORE_ASSERT(false, "Entity has no Tag Component");
		}
		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent" << YAML::BeginMap; // TransfomComponent

			auto& transform = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << transform.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << transform.Scale;

			out << YAML::EndMap; // TrasnformComponent
		}
		else
		{
			LD_CORE_ASSERT(false, "Entity has no Transform Component");
		}
		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent" << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << DirectX::XMConvertToDegrees(camera.GetPerspectiveVerticalFov());
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::EndMap; // Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap; // CameraComponent
		}
		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent" << YAML::BeginMap; // SpriteRendererComponent
			
			auto& spriteRenderer = entity.GetComponent<SpriteRendererComponent>();

			out << YAML::Key << "TexturePath" << YAML::Value << spriteRenderer.TexturePath.string();

			out << YAML::Key << "Color" << YAML::Value << spriteRenderer.Color;

			out << YAML::Key << "TilingFactor" << YAML::Value << spriteRenderer.TilingFactor;

			out << YAML::EndMap; // SpriteRendererComponent
		}

		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::SerializeToYamlFile(const std::filesystem::path& filePath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled Scene";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		// Reverse iterate when serializing otherwise the entity order will flip every time it's saved to a file
		auto view = m_Scene->m_Registry.view<entt::entity>();
		for (auto entityITE = view.rbegin(); entityITE != view.rend(); entityITE++)
		{
			auto entityID = *entityITE;

			Entity entity(entityID, m_Scene.get());
			if (!entity) { return; }

			SerializeEntity(out, entity);
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		auto dir = filePath.parent_path();
		if (!std::filesystem::exists(dir))
		{
			std::filesystem::create_directories(dir);
		}

		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeBinaryToFile(const std::filesystem::path& filePath)
	{
		LD_CORE_ASSERT(false, "Not implemented");
	}

	bool SceneSerializer::DeserializeFromYamlFile(const std::filesystem::path& filePath)
	{
		if (!std::filesystem::exists(filePath))
		{
			return false;
		}

		m_Scene->m_Registry.clear();
		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
		{
			return false;
		}

		std::string sceneName = data["Scene"].as<std::string>();
		LD_CORE_TRACE("Deserializing Scene {0}", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entityData : entities)
			{
				uint64_t uuid = entityData["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entityData["TagComponent"];
				if (tagComponent)
				{
					name = tagComponent["Tag"].as<std::string>();
				}

				LD_CORE_TRACE("Deserialized Entity with ID = {0}, name = {1}", uuid, name);

				Entity entity = m_Scene->CreateEntity(name);

				auto transformComponentData = entityData["TransformComponent"];
				if (transformComponentData)
				{
					auto& transformComponent = entity.GetComponent<TransformComponent>();
					transformComponent.Translation = transformComponentData["Translation"].as<DirectX::XMFLOAT3>();
					transformComponent.Rotation = transformComponentData["Rotation"].as<DirectX::XMFLOAT3>();
					transformComponent.Scale = transformComponentData["Scale"].as<DirectX::XMFLOAT3>();
				}

				auto cameraComponentData = entityData["CameraComponent"];
				if (cameraComponentData)
				{
					auto& cameraComponent = entity.AddComponent<CameraComponent>();

					auto cameraData = cameraComponentData["Camera"];
					auto& camera = cameraComponent.Camera;
					camera.SetProjectionType((SceneCamera::ProjectionType)cameraData["ProjectionType"].as<int>());

					camera.SetPerspectiveVerticalFov(DirectX::XMConvertToRadians(cameraData["PerspectiveFOV"].as<float>()));
					camera.SetPerspectiveNearClip(cameraData["PerspectiveNear"].as<float>());
					camera.SetPerspectiveFarClip(cameraData["PerspectiveFar"].as<float>());

					camera.SetOrthographicSize(cameraData["OrthographicSize"].as<float>());
					camera.SetOrthographicNearClip(cameraData["OrthographicNear"].as<float>());
					camera.SetOrthographicFarClip(cameraData["OrthographicFar"].as<float>());
					
					cameraComponent.Primary = cameraComponentData["Primary"].as<bool>();
					cameraComponent.FixedAspectRatio = cameraComponentData["FixedAspectRatio"].as<bool>();
				}

				auto spriteRendererComponentData = entityData["SpriteRendererComponent"];
				if (spriteRendererComponentData)
				{
					auto& spriteRendererComponent = entity.AddComponent<SpriteRendererComponent>();
					spriteRendererComponent.TexturePath = spriteRendererComponentData["TexturePath"].as<std::string>();

					if (std::filesystem::exists(spriteRendererComponent.TexturePath))
					{
						spriteRendererComponent.Texture = SubTexture2D::Create(Texture2D::Create(spriteRendererComponent.TexturePath));
					}

					spriteRendererComponent.Color = spriteRendererComponentData["Color"].as<DirectX::XMFLOAT4>();
					spriteRendererComponent.TilingFactor = spriteRendererComponentData["TilingFactor"].as<float>();
				}
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeBinaryFromFile(const std::filesystem::path& filePath)
	{
		LD_CORE_ASSERT(false, "Not implemented");
		return false;
	}

}