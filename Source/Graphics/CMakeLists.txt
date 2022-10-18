target_sources(acc3d PRIVATE
    Renderer.cpp
    RendererFactory.cpp
    ShaderCompiler.cpp
    ShaderLibrary.cpp
    RootSignatureLibrary.cpp
    ResultHandler.cpp
    Synchronizer.cpp
    ShaderReflection.cpp
    Drawable.cpp
    DrawableFactory.cpp
    LightContext.cpp
    Material.cpp
    RendererConfiguration.h
    Type.h)

add_subdirectory(Material)
add_subdirectory(Wrappers)
add_subdirectory(Memory)