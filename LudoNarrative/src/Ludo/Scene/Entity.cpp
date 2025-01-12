#include "ldpch.h"
#include "Entity.h"

namespace Ludo {

	Entity::Entity(entt::entity entityHandle, Scene* scene)
		: m_EntityHandle(entityHandle), m_Scene(scene)
	{
	}

}