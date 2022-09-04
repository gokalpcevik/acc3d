#include "Scene.h"
#include "Entity.h"

namespace acc3d::ECS
{
    auto Scene::CreateEntity() -> Entity
    {
        entt::entity e = m_Registry.create();
        m_Registry.emplace<TransformComponent>(e);
        return {this, e};
    }

    void Scene::Destroy(Entity entity)
    {
        m_Registry.destroy(entity);
    }

    void Scene::Update(float dt)
    {

    }

    void Scene::Draw(Graphics::Renderer &renderer, const FLOAT *clearColor)
    {
        renderer.Render(clearColor);
    }
}