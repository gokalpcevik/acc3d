#pragma once

#include <entt/entt.hpp>
#include "CommonComponents.h"
#include "CameraComponent.h"
#include "MeshRendererComponent.h"
#include "LightComponents.h"
#include "../AssetCore/MeshLibrary.h"

namespace acc3d
{
	namespace Graphics
	{
		class Renderer;
	}
}

namespace acc3d::ECS
{
    class Entity;

    class Scene
    {
    public:
        explicit Scene(Graphics::Renderer* pRenderer);

        Scene(Scene &&) = default;

        Scene(const Scene &) = delete;

        entt::registry const& GetEnTTRegistry() const;

        entt::registry& GetEnTTRegistryMutable();

        size_t GetCreatedEntityCount() const;

    	auto CreateEntity() -> Entity;

        void Destroy(Entity entity);

        void Update(float dt);

        template<typename T>
        auto GetComponent(entt::entity entity) -> T &;

        template<typename T, typename... Args>
        auto AddComponent(entt::entity entity, Args &&...args) -> T &;

        template<typename T>
        auto RemoveComponent(entt::entity entity) -> void;

        template<typename T>
        auto DestroyAllComponentsOfType() -> void;

    private:
    	void OnConstructMeshRendererComponent(entt::registry& registry, entt::entity entity) const;
    	void OnDestroyMeshRendererComponent(entt::registry& registry, entt::entity entity) const;

        void OnConstructDirectionalLightComponent(entt::registry& registry, entt::entity entity);
        void OnDestoryDirectionalLightComponent(entt::registry& registry, entt::entity entity);

    private:
        entt::registry m_Registry{};
        Graphics::Renderer* m_Renderer{ nullptr };
    };

    template<typename T>
    inline auto Scene::GetComponent(entt::entity entity) -> T &
    {
        return m_Registry.get<T>(entity);
    }

    template<typename T, typename... Args>
    inline auto Scene::AddComponent(entt::entity entity, Args &&...args) -> T &
    {
        return m_Registry.emplace<T>(entity, std::forward<Args>(args)...);
    }

    template<typename T>
    auto Scene::RemoveComponent(entt::entity entity) -> void
    {
        m_Registry.remove<T>(entity);
    }

    template <typename T>
    auto Scene::DestroyAllComponentsOfType() -> void
    {
        m_Registry.clear<T>();
    }
} //
