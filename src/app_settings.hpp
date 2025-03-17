#pragma once

#include <iostream>

#define APP_WINDOW_NAME     "Vulkan"
#define APP_WINDOW_WIDTH    800
#define APP_WINDOW_HEIGHT   600
#define APP_RESIZABLE_WINDOW

/* 
 * Utility Macros
 */
/* TODO: move these macros to an app utility-specific header */
#define APP_PRETTY_PRINT(str) std::cout << "🎆 ~~~ " << str << " ~~~ 🎆" << std::endl
#define DBG_ERR(msg) std::cout << "❌❌ " msg << std::endl
#define APP_DBG_ERR(msg) std::cout << "❌❌ " msg << std::endl
#define PRETTY_PRINT_CUSTOM(msg, symbol) std::cout << #symbol " ~~~ " << msg << " ~~~ " #symbol << std::endl
#define APP_PRETTY_PRINT_CUSTOM(msg, symbol) std::cout << #symbol " ~~~ " << msg << " ~~~ " #symbol << std::endl
#define CHECK() (std::cout << "✅ ✅ ✅" << std::endl)
#define APP_CHECK() (std::cout << "✅ ✅ ✅" << std::endl)

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

