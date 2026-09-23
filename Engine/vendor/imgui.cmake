set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)

if (NOT EXISTS ${IMGUI_DIR})
    include(FetchContent)
    FetchContent_Declare(
            imgui
            GIT_REPOSITORY https://github.com/ocornut/imgui.git
            GIT_TAG v1.92.9-docking
            SOURCE_DIR ${IMGUI_DIR}
    )
    FetchContent_MakeAvailable(imgui)
endif ()

add_library(imgui STATIC
        ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
        ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
        ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
)
target_include_directories(imgui PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${IMGUI_DIR}/misc/cpp
)