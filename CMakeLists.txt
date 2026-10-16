cmake_minimum_required(VERSION 3.24)

project(DirectX11Rendering LANGUAGES CXX)

add_executable(DirectX11Rendering
    src/Main.cpp
)

target_compile_features(DirectX11Rendering PRIVATE cxx_std_20)

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