#include "Application.h"

namespace acc3d
{
    namespace Core
    {
        auto AppStats::GetFramesPerSecond() const -> float
        {
            return 1.0f / ((float) m_FrameTime / (float) SDL_GetPerformanceFrequency());
        }

        auto AppStats::GetFrameTime() const -> float
        {
            return (float) m_FrameTime / (float) SDL_GetPerformanceFrequency() * 1000.0f;
        }

        auto AppStats::GetFrameTimeSeconds() const -> float
        {
            return (float) m_FrameTime / (float) SDL_GetPerformanceCounter();
        }

        auto Application::Get() -> Application &
        {
            static Application instance;
            return instance;
        }

        auto Application::Start() -> int32_t
        {
            Log::Init();
            m_Window = std::make_unique<Window>(1280, 720, "Accelerated Graphics 3D",
                                                SDL_WINDOW_RESIZABLE);
            if (m_Window->IsNull())
                return -1;

            m_Renderer = Graphics::RendererFactory::CreateRenderer(*m_Window);
            if (!m_Renderer)
                return -1;

            return Update();
        }

        auto Application::Update() -> int32_t
        {
            while (m_Running)
            {
                auto const counter = SDL_GetPerformanceCounter();
                m_Stats.m_FrameTime = counter - m_Stats.m_LastTickCount;
                m_Stats.m_LastTickCount = counter;

                while (m_Window->PollEvents())
                {
                    switch (m_Window->GetEvent().type)
                    {
                        case SDL_QUIT:
                        {
                            return 0;
                        }
                        case SDL_WINDOWEVENT:
                        {
                            switch (m_Window->GetEvent().window.event)
                            {
                                case SDL_WINDOWEVENT_RESIZED:
                                {
                                    int w = m_Window->GetEvent().window.data1;
                                    int h = m_Window->GetEvent().window.data2;
                                    m_Renderer->Resize(w, h);
                                    break;
                                }
                                default:
                                    break;
                            }
                        }
                        case SDL_KEYDOWN:
                        {
                            if (m_Window->GetEvent().key.keysym.sym == SDLK_ESCAPE)
                            {
                                m_Running = false;
                                break;
                            }
                        }
                    }
                }
                const FLOAT color[] = {0.1f, 0.1f, 0.7f, 1.0f};
                m_Renderer->Render(color);
            }
            SDL_Quit();
            return 0;
        }
    }
}
