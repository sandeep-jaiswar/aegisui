cmake_minimum_required(VERSION 3.20)
project(aegisui VERSION 0.1.0 LANGUAGES CXX)

# Require C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Static library target
add_library(aegisui STATIC
    src/scene_builder.cpp
)

target_include_directories(aegisui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Strict compiler warnings for both Clang and GCC
target_compile_options(aegisui PRIVATE
    -Wall
    -Wextra
    -Werror
)

# No runtime or OS dependencies - static library only
set_target_properties(aegisui PROPERTIES
    POSITION_INDEPENDENT_CODE OFF
)
