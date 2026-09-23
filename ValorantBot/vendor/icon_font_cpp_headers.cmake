set(ICON_FONT_CPP_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/vendor/IconFontCppHeaders)

if (NOT EXISTS ${ICON_FONT_CPP_HEADERS})
    include(FetchContent)
    FetchContent_Declare(
            IconFontCppHeaders
            GIT_REPOSITORY https://github.com/juliettef/IconFontCppHeaders.git
            GIT_TAG main
            SOURCE_DIR ${ICON_FONT_CPP_HEADERS}
    )
    FetchContent_MakeAvailable(IconFontCppHeaders)
endif ()
