#include "utils.hpp"

#include <iostream>
#include <filesystem>
#include <mach-o/dyld.h>

namespace fs = std::filesystem;

std::string utils_get_exe_path() {
    char buffer[PATH_MAX];
    uint32_t path_size = sizeof(buffer);

    if (_NSGetExecutablePath(buffer, &path_size) == -1) {
        std::cerr << "Something went wrong in retreiving the executable's path" << std::endl;
    };

    return std::string{buffer};
}

std::string utils_get_exe_dir() {
    char buffer[PATH_MAX];
    uint32_t path_size = sizeof(buffer);

    if (_NSGetExecutablePath(buffer, &path_size) == -1) {
        std::cerr << "Something went wrong in retreiving the executable's path" << std::endl;
    };

    fs::path in_path{std::string(buffer)};

    return in_path.parent_path();
}

