/*
 * General purpose utility functions/macro etc...
 */

#include <iostream>

/* TODO: move these macros to an app utility-specific header */
#define APP_PRETTY_PRINT(str) std::cout << "🎆 ~~~ " << str << " ~~~ 🎆" << std::endl
#define DBG_ERR(msg) std::cout << "❌❌ " msg << std::endl
#define APP_DBG_ERR(msg) std::cout << "❌❌ " msg << std::endl
#define APP_DBG_WARN(msg) std::cout << "⚠️ " << msg << std::endl
#define APP_PRETTY_PRINT_CUSTOM(msg, symbol) std::cout << symbol << " ~~~ " << msg << " ~~~ " << symbol << std::endl
#define APP_PRETTY_PRINT_CREATE(msg) APP_PRETTY_PRINT_CUSTOM(msg, "☀️")
#define APP_PRETTY_PRINT_ALLOC(msg) APP_PRETTY_PRINT_CUSTOM(msg, "🫧")
#define APP_PRETTY_PRINT_DESTROY(msg) APP_PRETTY_PRINT_CUSTOM(msg, "🌙")
#define CHECK() (std::cout << "✅ ✅ ✅" << std::endl)
#define APP_CHECK() (std::cout << "✅ ✅ ✅" << std::endl)
#define APP_PRINT_INFO(msg) std::cout << "ℹ️ " << msg << std::endl

template <typename T, typename... Args>
void app_dbg_print(T arg, Args... args) {
    if constexpr(sizeof...(args) != 0) {
        std::cout << arg << std::endl;
        app_dbg_print(args...);
    }
}

template <typename T>
void app_dbg_print_container(T& cont) {
    std::cout << "🎤 Printing container..." << std::endl;
    for (auto& item: cont) {
        std::cout << item << std::endl;
    }
    std::cout << "🎤 Finished printing container" << std::endl;
}

