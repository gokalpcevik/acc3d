target_sources(acc3d PRIVATE
    Renderer.cpp
    RendererFactory.cpp
    ShaderCompiler.cpp
    ShaderLibrary.cpp
    Device.cpp
    SwapChain.cpp
    CommandQueue.cpp
    CommandList.cpp
    CommandAllocator.cpp
    Resource.cpp
    RootSignature.cpp
    RootSignatureLibrary.cpp
    Fence.cpp
    DescriptorHeap.cpp
    ResultHandler.cpp
    InfoQueue.cpp
    Synchronizer.cpp
    PipelineState.cpp
    ShaderReflection.cpp
    Drawable.cpp
    LightContext.cpp
    Material.cpp
    RendererConfiguration.h
    Type.h)

add_subdirectory(Material)