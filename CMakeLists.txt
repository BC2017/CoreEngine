cmake_minimum_required(VERSION 3.22)

project(HFEngine
    VERSION 0.1.0
    DESCRIPTION "High fidelity C++ game engine foundation"
    LANGUAGES CXX
)

option(HFENGINE_BUILD_TESTS "Build engine tests" ON)
option(HFENGINE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

add_library(HFEngineCore STATIC
    src/Engine/Asset/AssetRegistry.cpp
    src/Engine/Core/Assert.cpp
    src/Engine/Core/Clock.cpp
    src/Engine/Core/Engine.cpp
    src/Engine/Core/CommandLine.cpp
    src/Engine/Core/FileSystem.cpp
    src/Engine/Core/JobSystem.cpp
    src/Engine/Core/Logger.cpp
    src/Engine/Renderer/FrameGraph.cpp
    src/Engine/RHI/BackendFactory.cpp
    src/Engine/RHI/Resource.cpp
    src/Engine/RHI/RendererBackend.cpp
    src/Engine/Core/Version.cpp
)

target_include_directories(HFEngineCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
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
        tests/unit/RhiContractTests.cpp
    )

    hfengine_add_test(HFEngineIntegrationTests integration
        tests/integration/EngineBootstrapTests.cpp
    )

    hfengine_add_test(HFEngineGpuSmokeTests gpu
        tests/integration/RhiGpuSmokeTests.cpp
    )
endif()
