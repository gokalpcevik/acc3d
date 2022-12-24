#pragma once
#include <SDL2/SDL.h>
#include <fmt/format.h>
#include <vector>
#include "Log.h"
#include "Window.h"
#include "Input.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/RendererFactory.h"
#include "../ECS/Scene.h"
#include "../ECS/Entity.h"
#include "../Graphics/Material.h"

namespace acc3d::Core
{
    struct AppStats
    {
        Uint64 m_LastTickCount = 0ULL;
        Uint64 m_FrameTime = 0ULL;

        [[nodiscard]] auto GetFramesPerSecond() const -> float;

        [[nodiscard]] auto GetFrameTime() const -> float;

        [[nodiscard]] auto GetFrameTimeSeconds() const -> float;
    };

    class Application
    {
    public:
        Application() = default;

        static auto Get() -> Application &;

        auto Start() -> int32_t;

    private:
        auto Update() -> int32_t;
        void HandleWindowEvent(Uint32 type);
        void CalculateAppStats();
        void DrawApplicationStats();

    private:
        AppStats m_Stats{};
        bool m_Running = true;
        std::unique_ptr<Window> m_Window;
        uint32_t m_WindowW = 1920U;
        uint32_t m_WindowH = 1080U;
        std::unique_ptr<Graphics::Renderer> m_Renderer;
        std::unique_ptr<ECS::Scene> m_Scene;

        uint32_t m_ViewportWidth;
        uint32_t m_ViewportHeight;

        ECS::Entity m_Spheres[4][4];

        ECS::Entity m_Camera;
        ECS::Entity m_Light;

        Eigen::Vector2i m_PrevCursorPosition { 0.0f, 0.0f};
    };
}
