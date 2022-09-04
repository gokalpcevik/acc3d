#include "Entity.h"

namespace acc3d::ECS
{
    Entity::Entity(Scene *pScene, entt::entity entity) : m_EntityHandle(entity), m_Scene(pScene)
    {

    }
} // namespace Q3D
