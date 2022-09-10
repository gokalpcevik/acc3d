cmake_minimum_required(VERSION 3.21)

project(acc3d)

find_package(SDL2 2.0.14 REQUIRED)
find_package(spdlog REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(directxtk12 CONFIG REQUIRED)
find_package(directxtex CONFIG REQUIRED)
find_package(directxmath CONFIG REQUIRED)
find_package(sparsehash CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

add_executable(${PROJECT_NAME} "Source/Main.cpp")

add_subdirectory("Source")

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        CXX_STANDARD 17
        )

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2::SDL2
    spdlog::spdlog
    Eigen3::Eigen
    fmt::fmt
    assimp::assimp
    EnTT::EnTT
    d3d12
    d3dcompiler
    dxgi
    dxguid
    Microsoft::DirectXTK12
    Microsoft::DirectXTex
    Microsoft::DirectXMath
    sparsehash::sparsehash
    yaml-cpp
    )

find_path(D3DX12_INCLUDE_DIRS "d3dx12.h")

target_include_directories(${PROJECT_NAME} PRIVATE ${D3DX12_INCLUDE_DIRS})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Material/RootSigDef
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Material/RootSigDef)