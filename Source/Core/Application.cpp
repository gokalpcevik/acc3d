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
                                            SDL_WINDOW_RESIZABLE|SDL_WINDOW_MAXIMIZED);
        if (m_Window->IsNull())
            return -1;

        m_Renderer = Graphics::RendererFactory::CreateRenderer(*m_Window);
        if (!m_Renderer)
            return -1;

        m_Scene = std::make_unique<ECS::Scene>(m_Renderer.get());


    	this->m_Donut = m_Scene->CreateEntity();
        auto donutAssetId = Asset::MeshLibrary()("Assets/donut.obj");
        Graphics::RootSignatureInitializer rootSigInit
    	{ "Materials/RootSignatures/diffuse_rootsig.yml",
    	   Graphics::ACC3D_DIFFUSE_ROOT_SIGNATURE };
        m_Donut.AddComponent<ECS::MeshRendererComponent>(donutAssetId,rootSigInit);


        auto sphereAssetId = Asset::MeshLibrary()("Assets/sphere.obj");
    	auto sphere = m_Scene->CreateEntity();
        sphere.AddComponent<ECS::MeshRendererComponent>(sphereAssetId, rootSigInit);

        auto& tc = m_Donut.GetComponent<ECS::TransformComponent>();
        tc.Translation = { -5.0f,0.0f,0.0f };

        auto cubeAssetId = Asset::MeshLibrary()("Assets/cube.obj");
        auto cube = m_Scene->CreateEntity();
    	cube.AddComponent<ECS::MeshRendererComponent>(cubeAssetId, rootSigInit);
        auto& ctc = cube.GetComponent<ECS::TransformComponent>();
        ctc.Translation = { +5.0f,0.0f,0.0f };


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
        Eigen::Vector3f direction{ 1.0f , -0.5f, +1.0f };
        dlc.Direction = *(DirectX::XMFLOAT3*)&(direction.normalized());
        dlc.Color = { 1.0f,1.0f, 0.85f };

        return Update();
    }

    auto Application::Update() -> int32_t
    {
        while (m_Running)
        {
            this->CalculateAppStats();

            while (m_Window->PollEvents())
            {
                this->HandleWindowEvent(m_Window->GetEvent().type);
            }

            if (Input::IsKeyPressed(SDL_SCANCODE_F2))
            {
                m_Window->SetTitle(fmt::format("Direct3D12 Renderer - FPS: {0:.1f}, Frame Time: {1:.1f}", m_Stats.GetFramesPerSecond(), m_Stats.GetFrameTime()).c_str());
            }

            if (Input::IsKeyPressed(SDL_SCANCODE_ESCAPE))
            {
                m_Running = false;
            }

            if (Input::IsKeyPressed(SDL_SCANCODE_F1))
            {
                m_Scene->DestroyAllComponentsOfType<ECS::MeshRendererComponent>();
            }


            auto&& cc = m_Camera.GetComponent<ECS::CameraComponent>();

            DirectX::XMVECTOR const focusPoint = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

            static DirectX::XMFLOAT4 EyePosition = { 0.0f,0.0f,-5.0f,1.0f };

            if (Input::IsKeyPressed(SDL_SCANCODE_UP))
                EyePosition.y += 0.05f;
            else if (Input::IsKeyPressed(SDL_SCANCODE_DOWN))
                EyePosition.y -= 0.05f;


            const DirectX::XMVECTOR eyePositionVec = DirectX::XMLoadFloat4(&EyePosition);
            const DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0, 1, 0, 0);
            cc.ViewMatrix = DirectX::XMMatrixLookAtLH(eyePositionVec, focusPoint, upDirection);

          
            FLOAT constexpr clearColor[] = { 0.05f, 0.05f, 0.05f, 1.0f };

            m_Renderer->Clear(clearColor);
            m_Renderer->RenderScene(*m_Scene);
            m_Renderer->Present();
        }
        SDL_Quit();
        return 0;
    }

    void Application::HandleWindowEvent(Uint32 type)
    {
        switch (type)
        {
        case SDL_QUIT:
        {
            m_Running = false;
            return;
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

    void Application::CalculateAppStats()
    {
        auto const counter = SDL_GetPerformanceCounter();
        m_Stats.m_FrameTime = counter - m_Stats.m_LastTickCount;
        m_Stats.m_LastTickCount = counter;
    }
}
