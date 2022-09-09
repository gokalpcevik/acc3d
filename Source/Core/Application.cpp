#include "Application.h"

namespace acc3d::Core
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
        m_Window = std::make_unique<Window>(1280, 720, "Direct3D12 Renderer",
                                            SDL_WINDOW_RESIZABLE);
        if (m_Window->IsNull())
            return -1;

        m_Renderer = Graphics::RendererFactory::CreateRenderer(*m_Window);
        if (!m_Renderer)
            return -1;

        m_Scene = std::make_unique<ECS::Scene>(m_Renderer.get());

    	this->m_Donut = m_Scene->CreateEntity();
        auto id = Asset::MeshLibrary()("Assets/donut.obj");
        m_Donut.AddComponent<ECS::MeshRendererComponent>(id);
        auto& tc = m_Donut.GetComponent<ECS::TransformComponent>();
        tc.Rotation = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, 90.0f, 90.0f);

        this->m_Camera = m_Scene->CreateEntity();
        auto&cc = m_Camera.AddComponent<ECS::CameraComponent>(true);

    	const DirectX::XMVECTOR eyePosition = DirectX::XMVectorSet(0, 0, -5, 1);
        const DirectX::XMVECTOR focusPoint = DirectX::XMVectorSet(0, 0, 0, 1);
        const DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0, 1, 0, 0);
        // Look at the donut!
        cc.ViewMatrix = DirectX::XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);

        m_Light = m_Scene->CreateEntity();
        m_Light2 = m_Scene->CreateEntity();


        auto& dlc = m_Light.AddComponent<ECS::DirectionalLightComponent>();
        dlc.Intensity = 1.0f;
        dlc.Direction = { -0.485f , +0.727f , -0.485f };
        dlc.Color = { 0.0f,0.0f,1.0f };


        auto& dlc2 = m_Light2.AddComponent<ECS::DirectionalLightComponent>();
        dlc2.Intensity = 1.0f;
        dlc2.Direction = { +0.485f , +0.727f , +0.485f };
        dlc2.Color = { 1.0f,0.0f,0.0f };

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
                }
            }
            
            if (Input::IsKeyPressed(SDL_SCANCODE_ESCAPE))
            {
                m_Running = false;
            }

            if(Input::IsKeyPressed(SDL_SCANCODE_F1))
            {
                m_Scene->DestroyAllComponentsOfType<ECS::MeshRendererComponent>();
            }

            auto&& cc = m_Camera.GetComponent<ECS::CameraComponent>();


            if(Input::IsKeyPressed(SDL_SCANCODE_R))
            {
                cc.FOVHalfAngle += 0.001f * m_Stats.GetFrameTime();
            }
        	else if(Input::IsKeyPressed(SDL_SCANCODE_T))
            {
                cc.FOVHalfAngle -= 0.001f * m_Stats.GetFrameTime();
            }


            m_Window->SetTitle(fmt::format("FPS: {0:.2f}, Frame Time: {1:.2f}", m_Stats.GetFramesPerSecond(), m_Stats.GetFrameTime()).c_str());
            


            auto& tc = m_Donut.GetComponent<ECS::TransformComponent>();

            static float Roll = 0.0f;
            static float Pitch = 0.0f;
            static float Yaw = 0.0f;

            if (Input::IsKeyPressed(SDL_SCANCODE_1))
                Roll += 0.01f;
            if (Input::IsKeyPressed(SDL_SCANCODE_2))
                Roll -= 0.01f;

            if (Input::IsKeyPressed(SDL_SCANCODE_3))
                Pitch += 0.01f;
            if (Input::IsKeyPressed(SDL_SCANCODE_4))
                Pitch -= 0.01f;

            if (Input::IsKeyPressed(SDL_SCANCODE_5))
                Yaw += 0.01f;
            if (Input::IsKeyPressed(SDL_SCANCODE_6))
                Yaw -= 0.01f;


            tc.Rotation = DirectX::XMQuaternionRotationRollPitchYaw(Roll , Pitch , Yaw);


            const FLOAT clearColor[] = {0.1f, 0.1f, 0.1f, 1.0f};

            m_Renderer->Clear(clearColor);
            m_Renderer->RenderScene(*m_Scene);
            m_Renderer->Present();
        }
        SDL_Quit();
        return 0;
    }
}
