project(Engine LANGUAGES CXX)

include(vendor/imgui.cmake)
include(vendor/spdlog.cmake)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_library(${PROJECT_NAME} ${SOURCE_FILES})

if (WIN32)
    add_definitions(-DPLATFORM_WINDOWS -DNOMINMAX)
elseif (UNIX AND NOT APPLE)
    add_definitions(-DPLATFORM_LINUX)
elseif (APPLE)
    add_definitions(-DPLATFORM_MACOS)
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC src)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE
        d3d11
        d3dcompiler
        Dwmapi
        Shlwapi
)
