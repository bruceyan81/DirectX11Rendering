cmake_minimum_required(VERSION 3.24)

project(DirectX11Rendering LANGUAGES CXX)

add_executable(DirectX11Rendering WIN32
    src/Main.cpp
    src/Application/Application.cpp
    src/Platform/Application.manifest
    src/Platform/Window/Window.cpp
)

target_include_directories(DirectX11Rendering PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_features(DirectX11Rendering PRIVATE cxx_std_20)

# Windows SDK configuration
target_compile_definitions(DirectX11Rendering PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    UNICODE
    _UNICODE
    WINVER=0x0A00
    _WIN32_WINNT=0x0A00
)

target_link_libraries(DirectX11Rendering PRIVATE
    user32
)

set_target_properties(DirectX11Rendering PROPERTIES
    CXX_EXTENSIONS OFF
)

if(MSVC)
    target_compile_options(DirectX11Rendering PRIVATE
        /W4
        /permissive-
        /Zc:__cplusplus
        /utf-8
    )
endif()
