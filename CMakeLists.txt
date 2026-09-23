cmake_minimum_required(VERSION 3.30)

project(ValorantBot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DIMGUI_USE_WCHAR32 -DIMGUI_DEFINE_MATH_OPERATORS -DENGINE_IMGUI)

add_subdirectory(Engine)
add_subdirectory(ValorantBot)
