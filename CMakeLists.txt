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

# Test executables
add_executable(ids_test tests/ids_test.cpp)
target_link_libraries(ids_test PRIVATE aegisui)
target_compile_options(ids_test PRIVATE
    -Wall
    -Wextra
    -Werror
)

add_executable(layout_test tests/layout_test.cpp)
target_link_libraries(layout_test PRIVATE aegisui)
target_compile_options(layout_test PRIVATE
    -Wall
    -Wextra
    -Werror
)

add_executable(style_test tests/style_test.cpp)
target_link_libraries(style_test PRIVATE aegisui)
target_compile_options(style_test PRIVATE
    -Wall
    -Wextra
    -Werror
)

add_executable(node_test tests/node_test.cpp)
target_link_libraries(node_test PRIVATE aegisui)
target_compile_options(node_test PRIVATE
    -Wall
    -Wextra
    -Werror
)

add_executable(text_node_test tests/text_node_test.cpp)
target_link_libraries(text_node_test PRIVATE aegisui)
target_compile_options(text_node_test PRIVATE
    -Wall
    -Wextra
    -Werror
)

add_executable(grid_node_test tests/grid_node_test.cpp)
target_link_libraries(grid_node_test PRIVATE aegisui)
target_compile_options(grid_node_test PRIVATE
    -Wall
    -Wextra
    -Werror
)

add_executable(scene_test tests/scene_test.cpp)
target_link_libraries(scene_test PRIVATE aegisui)
target_compile_options(scene_test PRIVATE
    -Wall
    -Wextra
    -Werror
)

# Enable testing
enable_testing()
add_test(NAME ids_test COMMAND ids_test)
add_test(NAME layout_test COMMAND layout_test)
add_test(NAME style_test COMMAND style_test)
add_test(NAME node_test COMMAND node_test)
add_test(NAME text_node_test COMMAND text_node_test)
add_test(NAME grid_node_test COMMAND grid_node_test)
add_test(NAME scene_test COMMAND scene_test)
