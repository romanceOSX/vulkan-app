add_library(
    utils
    src/utils.cpp
)

set(
    public_headers
    include/utils.hpp
)

target_compile_features(utils PUBLIC cxx_std_23)

target_include_directories(
    utils
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

