#include "Application.h"
#include "../Graphics/Memory/DescriptorAllocatorPage.h"

using namespace DirectX;

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
        std::string windowTitle{};
#if defined(DEBUG) || defined(_DEBUG)
        windowTitle = fmt::format("ACC3D Graphics Engine - x64 Debug");
#else
        windowTitle = fmt::format("ACC3D Graphics Engine - x64 Release");
#endif

        m_Window = std::make_unique<Window>(1280, 720, windowTitle.c_str(),
                                            SDL_WINDOW_RESIZABLE|SDL_WINDOW_MAXIMIZED);
        if (m_Window->IsNull())
            return -1;

        m_Renderer = Graphics::RendererFactory::CreateRenderer(*m_Window);
        if (!m_Renderer)
            return -1;

        m_Scene = std::make_unique<ECS::Scene>(m_Renderer.get());


    	
        auto sphereAssetId = Asset::MeshLibrary()("Assets/sphere.obj");
        Graphics::RootSignatureInitializer rootSigInit
    	{ "Materials/RootSignatures/diffuse_rootsig.yml",
    	   static_cast<Graphics::RootSignatureId>(Graphics::ROOT_SIG_ENTRY_VALUE::DIFFUSE_ROOT_SIGNATURE)};


        for (size_t i = 0; i < 4; ++i)
        {
            for (size_t j = 0; j < 4; ++j)
            {
                m_Spheres[i][j] = m_Scene->CreateEntity();
                m_Spheres[i][j].AddComponent<ECS::MeshRendererComponent>(sphereAssetId, rootSigInit);

                auto& tc = m_Spheres[i][j].GetComponent<ECS::TransformComponent>();

                tc.Translation = { static_cast<float>((i - 2.0f) * 2.5f) , 0.0f , static_cast<float>((j - 2.0f) * 2.5f) };
            }
        }

        this->m_Camera = m_Scene->CreateEntity();
        auto&cc = m_Camera.AddComponent<ECS::CameraComponent>(true);

    	const DirectX::XMVECTOR eyePosition = DirectX::XMVectorSet(0, 7.5f, -10, 1);
        const DirectX::XMVECTOR focusPoint = DirectX::XMVectorSet(-1, 0, -2, 1);
        const DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0, 1, 0, 0);
        // Look at the donut!
        cc.SetViewMatrix(DirectX::XMMatrixLookAtLH(eyePosition, focusPoint, upDirection));

        m_Light = m_Scene->CreateEntity();

        auto& dlc = m_Light.AddComponent<ECS::DirectionalLightComponent>();
        dlc.Intensity = 1.0f;
        Eigen::Vector3f direction{ 1.0f , -0.75f, +1.0f };
        dlc.Direction = *(DirectX::XMFLOAT3*)&(direction.normalized());
        dlc.Color = { 0.85f,0.85f, 1.00f };
        m_ViewportWidth = m_Window->GetSurfaceWidth();
        m_ViewportHeight = m_Window->GetSurfaceHeight();


        using Graphics::Memory::DescriptorAllocator;

        DescriptorAllocator* pAllocator = new DescriptorAllocator(m_Renderer->GetDevice()->GetD3D12DevicePtr(),
                                                                  D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 256);
        return Update();
    }

    auto Application::Update() -> int32_t
    {
        while (m_Running)
        {
            static bool IsWindowHovered = false;

            this->CalculateAppStats();

            while (m_Window->PollEvents())
            {
                this->HandleWindowEvent(m_Window->GetEvent().type);
                ImGui_ImplSDL2_ProcessEvent(&m_Window->GetEvent());
            }

            if (Input::IsKeyPressed(SDL_SCANCODE_ESCAPE))
            {
                m_Running = false;
            }

            if (Input::IsKeyPressed(SDL_SCANCODE_F1))
            {
                m_Scene->DestroyAllComponentsOfType<ECS::MeshRendererComponent>();
            }

            auto& cc = m_Camera.GetComponent<ECS::CameraComponent>();

            Vector2i CurrentCursorPos = Input::GetCursorPosition();
            Vector2i DeltaCursorPos = CurrentCursorPos - m_PrevCursorPosition;
            m_PrevCursorPosition = CurrentCursorPos;

        	if(Input::IsLeftMouseButtonPressed() && !IsWindowHovered)
        	{
                float deltaAngleX = (2 * 3.14159f / static_cast<float>(m_ViewportWidth));
                float xAngle = DeltaCursorPos[0] * deltaAngleX;

                XMVECTOR pivot = DirectX::XMVectorSet(-1.0f, 0.0f, -2.0f, 1.0f);
                XMVECTOR position = cc.GetCameraPosition();

                position = XMVectorAdd(XMVector3Transform((XMVectorSubtract(position, pivot)), XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), xAngle)), pivot);

                cc.SetViewMatrix(XMMatrixLookAtLH(position, pivot, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
        	}

            ImGui_ImplDX12_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            this->DrawStatsAndSettings();
            IsWindowHovered = ImGui::IsWindowFocused();
        	ImGui::End();

        	ImGui::Render();


            m_Renderer->Clear(m_RendererClearColor);
            m_Renderer->RenderScene(*m_Scene);
            m_Renderer->RenderImGui();
            m_Renderer->Present();
        }
        m_Scene->Shutdown();
        m_Renderer->Shutdown();
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

    void Application::DrawStatsAndSettings()
    {
        ImGui::Begin("Stats", 0,  
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings);

        auto const stats = m_Renderer->GetAllocatorStats();
        if(ImGui::BeginTabBar("#ACC3D"))
        {
            if(ImGui::BeginTabItem("Statistics"))
            {
                ImGui::TextColored({ 0.76f,0.76f,1.0f,1.0f }, "Renderer");
                ImGui::Text("FPS: %0.1f", m_Stats.GetFramesPerSecond());
                ImGui::Text("Frame Time: %0.1f ms", m_Stats.GetFrameTime());
                ImGui::Text("Draw Calls: %d", m_Renderer->GetFrameDrawCallCount());
                ImGui::Text("Frame Number: %d", m_Renderer->GetFrameNumber());

                ImGui::Separator();

                ImGui::TextColored({ 0.76f,0.76f,1.0f,1.0f }, "D3DMA GPUOpen");
                ImGui::Text("Allocation Count: %d", stats.Total.AllocationCount);
                ImGui::Text("Allocations Total: %dMB", (stats.Total.UsedBytes + stats.Total.UnusedBytes) / 1024 / 1024);
                ImGui::Text("Used Amount: %dMB", stats.Total.UsedBytes / 1024 / 1024);
                ImGui::Text("Unused Amount: %dMB", stats.Total.UnusedBytes / 1024 / 1024);
                ImGui::Text("Block Count: %d", stats.Total.BlockCount);
                ImGui::EndTabItem();
            }
            if(ImGui::BeginTabItem("Settings"))
            {
                ImGui::ColorPicker4("Clear Color", m_RendererClearColor);

                auto& dlc = m_Light.GetComponent<ECS::DirectionalLightComponent>();
                ImGui::ColorPicker3("Light Color", (float*) & dlc.Color);
                ImGui::SliderFloat("Light Intensity", &dlc.Intensity, 0.0f, 5.0f);


                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
    }
}
