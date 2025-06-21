#include <iostream>
#include "utils.hpp"

void _test_current_exe_path() {
    std::cout << "testing current_exe_path" << std::endl;
    std::cout << utils_get_exe_path() << std::endl;
}

int main (int argc, char *argv[]) {
    _test_current_exe_path();
    return 0;
}

