#include <iostream>
#include "utils.hpp"

void _test_current_exe_path() {
    std::cout << "testing utils_get_exe_path" << std::endl;
    std::cout << utils_get_exe_path() << std::endl;
}

void _test_current_exe_dir() {
    std::cout << "testing utils_get_exe_dir" << std::endl;
    std::cout << utils_get_exe_dir() << std::endl;
}


int main (int argc, char *argv[]) {
    _test_current_exe_path();
    _test_current_exe_dir();
    return 0;
}

