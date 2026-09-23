#pragma once

#include <sstream>

#define BIT(x) (1 << x)
#define BIND_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

template <class... Ts>
struct overloads : Ts... {
    using Ts::operator()...;
};

#define IMPLEMENT_FORMATTER(T)                                                         \
    template <>                                                                        \
    struct std::formatter<T, char> : std::formatter<std::string_view> {                \
        auto format(const T& obj, std::format_context& ctx) const                      \
        {                                                                              \
            std::ostringstream oss;                                                    \
            oss << obj;                                                                \
            std::string s = oss.str();                                                 \
            return std::formatter<std::string_view>::format(std::string_view(s), ctx); \
        }                                                                              \
    }

#include <Engine/Core/Assert.h>
#include <Engine/Core/Log.h>