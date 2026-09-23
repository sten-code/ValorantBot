#pragma once

#include <Engine/Core/Core.h>

#include <filesystem>

#ifdef ENABLE_ASSERTS
        // Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
        // provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
        #define INTERNAL_ASSERT_IMPL(check, msg, ...) { if(!(check)) { ERR(msg, __VA_ARGS__); DEBUGBREAK(); } }
        #define INTERNAL_ASSERT_WITH_MSG(check, ...) INTERNAL_ASSERT_IMPL(check, "Assertion failed: {0}", __VA_ARGS__)
        #define INTERNAL_ASSERT_NO_MSG(check) INTERNAL_ASSERT_IMPL(check, "Assertion '{0}' failed at {1}:{2}", STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

        #define INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
        #define INTERNAL_ASSERT_GET_MACRO(...) EXPAND_MACRO( INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, INTERNAL_ASSERT_WITH_MSG, INTERNAL_ASSERT_NO_MSG) )

        // Currently accepts at least the condition and one additional parameter (the message) being optional
        #define ASSERT(...) EXPAND_MACRO( INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#else
        #define ASSERT(...)
#endif