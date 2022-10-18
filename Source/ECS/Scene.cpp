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

        /*
         * We will have a pleasant way (hopefully) handle light information soon.
         */
		m_Registry.on_construct<DirectionalLightComponent>().connect<&Scene::OnConstructDirectionalLightComponent>(this);
        m_Registry.on_destroy<DirectionalLightComponent>().connect<&Scene::OnDestoryDirectionalLightComponent>(this);
	}

	entt::registry const& Scene::GetEnTTRegistry() const
	{
        return m_Registry;
	}

	entt::registry& Scene::GetEnTTRegistryMutable()
	{
        return m_Registry;
	}

	size_t Scene::GetCreatedEntityCount() const
	{
        return m_Registry.size();
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

    void Scene::Shutdown()
    {
        this->DestroyAllComponentsOfType<MeshRendererComponent>();
    }

    void Scene::OnConstructMeshRendererComponent(entt::registry& registry, entt::entity entity)
    {
        auto& meshRendererComponent = registry.get<MeshRendererComponent>(entity);
        m_Renderer->RegisterMeshRendererComponentDrawable(meshRendererComponent.MeshAssetId,
            RIDAccessor()(meshRendererComponent), 
            meshRendererComponent.RootSignatureDescription);
        m_Registry.sort<MeshRendererComponent>([](const MeshRendererComponent& lhs, const MeshRendererComponent& rhs)
        {
                return lhs.RootSignatureDescription.RootSignatureId > rhs.RootSignatureDescription.RootSignatureId;
        });
      
    }

    void Scene::OnDestroyMeshRendererComponent(entt::registry& registry, entt::entity entity) const
    {
        m_Renderer->DeregisterMeshRendererComponentDrawable(
	    RIDAccessor()(registry.get<MeshRendererComponent>(entity)));
    }

    void Scene::OnConstructDirectionalLightComponent(entt::registry& registry, entt::entity entity)
    {
        

    }

    void Scene::OnDestoryDirectionalLightComponent(entt::registry& registry, entt::entity entity)
    {
    }
}
