cmake_minimum_required(VERSION 3.22)

project(HFEngine
    VERSION 0.1.0
    DESCRIPTION "High fidelity C++ game engine foundation"
    LANGUAGES CXX
)

find_program(HFENGINE_SLANGC slangc HINTS "$ENV{VULKAN_SDK}/Bin")

option(HFENGINE_BUILD_TESTS "Build engine tests" ON)
option(HFENGINE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

function(hfengine_seed_vulkan_sdk_cache)
    if(NOT DEFINED ENV{VULKAN_SDK})
        return()
    endif()

    file(TO_CMAKE_PATH "$ENV{VULKAN_SDK}" HFENGINE_VULKAN_SDK)

    foreach(candidate
        "${HFENGINE_VULKAN_SDK}/Include"
        "${HFENGINE_VULKAN_SDK}/include"
    )
        if(NOT Vulkan_INCLUDE_DIR AND EXISTS "${candidate}/vulkan/vulkan.h")
            set(Vulkan_INCLUDE_DIR "${candidate}" CACHE PATH "Path to Vulkan headers")
            break()
        endif()
    endforeach()

    foreach(candidate
        "${HFENGINE_VULKAN_SDK}/Lib/vulkan-1.lib"
        "${HFENGINE_VULKAN_SDK}/lib/vulkan-1.lib"
    )
        if(NOT Vulkan_LIBRARY AND EXISTS "${candidate}")
            set(Vulkan_LIBRARY "${candidate}" CACHE FILEPATH "Path to Vulkan loader library")
            break()
        endif()
    endforeach()
endfunction()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/imgui.cpp")
    message(FATAL_ERROR "Dear ImGui submodule is missing. Run: git submodule update --init --recursive")
endif()

add_library(HFEngineCore STATIC
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_dx12.cpp
    external/imgui/backends/imgui_impl_vulkan.cpp
    external/imgui/backends/imgui_impl_win32.cpp
    src/Engine/Asset/AssetRegistry.cpp
    src/Engine/Core/Assert.cpp
    src/Engine/Core/Clock.cpp
    src/Engine/Core/Engine.cpp
    src/Engine/Core/CommandLine.cpp
    src/Engine/Core/FileSystem.cpp
    src/Engine/Core/JobSystem.cpp
    src/Engine/Core/Logger.cpp
    src/Engine/Math/LinearAlgebra.cpp
    src/Engine/Platform/Win32Window.cpp
    src/Engine/Renderer/Camera.cpp
    src/Engine/Renderer/FrameGraph.cpp
    src/Engine/Renderer/SandboxMesh.cpp
    src/Engine/RHI/BackendFactory.cpp
    src/Engine/RHI/CommandList.cpp
    src/Engine/RHI/DX12/DX12SandboxRenderer.cpp
    src/Engine/RHI/Pipeline.cpp
    src/Engine/RHI/Resource.cpp
    src/Engine/RHI/RendererBackend.cpp
    src/Engine/RHI/Vulkan/VulkanSandboxRenderer.cpp
    src/Engine/Tools/RuntimeDebugOverlay.cpp
    src/Engine/Core/Version.cpp
)

target_include_directories(HFEngineCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
)

target_compile_features(HFEngineCore PUBLIC cxx_std_20)

target_compile_definitions(HFEngineCore
    PUBLIC
        HFENGINE_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        HFENGINE_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        HFENGINE_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

if(MSVC)
    target_compile_options(HFEngineCore PRIVATE /W4 /permissive-)
    if(HFENGINE_WARNINGS_AS_ERRORS)
        target_compile_options(HFEngineCore PRIVATE /WX)
    endif()
else()
    target_compile_options(HFEngineCore PRIVATE -Wall -Wextra -Wpedantic)
    if(HFENGINE_WARNINGS_AS_ERRORS)
        target_compile_options(HFEngineCore PRIVATE -Werror)
    endif()
endif()

if(WIN32)
    hfengine_seed_vulkan_sdk_cache()
    find_package(Vulkan REQUIRED)

    target_link_libraries(HFEngineCore
        PUBLIC
            d3d12
            dxgi
            d3dcompiler
            Vulkan::Vulkan
    )
endif()

add_executable(HFEngineSandbox
    src/Sandbox/Main.cpp
)

target_link_libraries(HFEngineSandbox
    PRIVATE
        HFEngineCore
)

target_compile_features(HFEngineSandbox PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(HFEngineSandbox PRIVATE /W4 /permissive-)
    if(HFENGINE_WARNINGS_AS_ERRORS)
        target_compile_options(HFEngineSandbox PRIVATE /WX)
    endif()
else()
    target_compile_options(HFEngineSandbox PRIVATE -Wall -Wextra -Wpedantic)
    if(HFENGINE_WARNINGS_AS_ERRORS)
        target_compile_options(HFEngineSandbox PRIVATE -Werror)
    endif()
endif()

function(hfengine_compile_mesh_shaders target)
    if(NOT HFENGINE_SLANGC)
        message(FATAL_ERROR "slangc was not found. Install the Vulkan SDK or add slangc to PATH.")
    endif()

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${HFENGINE_SLANGC}
            ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Mesh.slang
            -entry VSMain
            -profile vs_6_0
            -target dxil
            -o $<TARGET_FILE_DIR:${target}>/Mesh.vs.dxil
        COMMAND ${HFENGINE_SLANGC}
            ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Mesh.slang
            -entry PSMain
            -profile ps_6_0
            -target dxil
            -o $<TARGET_FILE_DIR:${target}>/Mesh.ps.dxil
        COMMAND ${HFENGINE_SLANGC}
            ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Mesh.slang
            -entry VSMain
            -profile glsl_450
            -target spirv
            -DHFENGINE_VULKAN
            -o $<TARGET_FILE_DIR:${target}>/Mesh.vert.spv
        COMMAND ${HFENGINE_SLANGC}
            ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Mesh.slang
            -entry PSMain
            -profile glsl_450
            -target spirv
            -DHFENGINE_VULKAN
            -o $<TARGET_FILE_DIR:${target}>/Mesh.frag.spv
        VERBATIM
    )
endfunction()

hfengine_compile_mesh_shaders(HFEngineSandbox)

if(HFENGINE_BUILD_TESTS)
    enable_testing()

    add_library(HFEngineTestHarness STATIC
        tests/harness/TestHarness.cpp
    )

    target_include_directories(HFEngineTestHarness
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/harness
    )

    target_compile_features(HFEngineTestHarness PUBLIC cxx_std_20)

    function(hfengine_add_test target label)
        add_executable(${target} ${ARGN})
        target_link_libraries(${target}
            PRIVATE
                HFEngineCore
                HFEngineTestHarness
        )

        if(MSVC)
            target_compile_options(${target} PRIVATE /W4 /permissive-)
        else()
            target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
        endif()

        add_test(NAME ${target} COMMAND ${target})
        set_tests_properties(${target} PROPERTIES LABELS ${label})
    endfunction()

    hfengine_add_test(HFEngineUnitTests unit
        tests/unit/AssetPipelineTests.cpp
        tests/unit/CoreSystemsTests.cpp
        tests/unit/CoreVersionTests.cpp
        tests/unit/FrameGraphTests.cpp
        tests/unit/MathCameraTests.cpp
        tests/unit/RhiContractTests.cpp
    )

    hfengine_add_test(HFEngineIntegrationTests integration
        tests/integration/EngineBootstrapTests.cpp
    )

    hfengine_add_test(HFEngineGpuSmokeTests gpu
        tests/integration/RhiGpuSmokeTests.cpp
    )

    hfengine_compile_mesh_shaders(HFEngineGpuSmokeTests)
endif()
