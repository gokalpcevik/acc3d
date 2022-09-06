#include "Scene.h"
#include "Entity.h"
#include "../Graphics/Renderer.h"

namespace acc3d::ECS
{

	Scene::Scene(Graphics::Renderer* pRenderer)
		: m_Renderer(pRenderer)
	{
        m_Registry.on_construct<MeshRendererComponent>().connect<&Scene::OnConstructMeshRendererComponent>(this);
        m_Registry.on_destroy<MeshRendererComponent>().connect<&Scene::OnDestroyMeshRendererComponent>(this);
	}

	entt::registry const& Scene::GetEnTTRegistry() const
	{
        return m_Registry;
	}

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

    void Scene::OnConstructMeshRendererComponent(entt::registry& registry, entt::entity entity) const
    {
        auto& meshRendererComponent = registry.get<MeshRendererComponent>(entity);
        auto& id = RendererIdAccessor()(meshRendererComponent);

		id = Graphics::Renderer::GenerateRendererId();
        m_Renderer->RegisterMeshRendererComponentDrawable(id,meshRendererComponent.MeshAssetId);
    }

    void Scene::OnDestroyMeshRendererComponent(entt::registry& registry, entt::entity entity) const
    {
        m_Renderer->DeregisterMeshRendererComponentDrawable(
	        RendererIdAccessor()(registry.get<MeshRendererComponent>(entity)));
    }
}
