set(SPDLOG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog)

if (NOT EXISTS ${SPDLOG_DIR})
    include(FetchContent)
    FetchContent_Declare(
            spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG v1.12.0
            SOURCE_DIR ${SPDLOG_DIR}
    )
    FetchContent_MakeAvailable(spdlog)
else ()
    add_subdirectory(${SPDLOG_DIR})
endif ()