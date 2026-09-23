set(HIDAPI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/hidapi)

if (NOT EXISTS ${HIDAPI_DIR})
    include(FetchContent)
    FetchContent_Declare(
            hidapi
            GIT_REPOSITORY https://github.com/libusb/hidapi.git
            GIT_TAG hidapi-0.15.0
            SOURCE_DIR ${HIDAPI_DIR}
    )
    FetchContent_MakeAvailable(hidapi)
else ()
    add_subdirectory(${HIDAPI_DIR})
endif ()