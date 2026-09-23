project(ValorantBot LANGUAGES CXX)

find_package(OpenCV REQUIRED)

include(vendor/hidapi.cmake)
include(vendor/icon_font_cpp_headers.cmake)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC src ${ICON_FONT_CPP_HEADERS})
target_link_libraries(${PROJECT_NAME} PUBLIC Engine opencv_core opencv_imgproc hidapi)
target_link_libraries(${PROJECT_NAME} PRIVATE
        d3d11
        d3dcompiler
        Dwmapi
        Shlwapi
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/Resources
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources
)