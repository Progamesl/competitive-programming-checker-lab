cmake_minimum_required(VERSION 3.15)
project(checker_lab CXX)

# ─── Language standard ────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ─── Compiler warnings ────────────────────────────────────────────────────────
if(MSVC)
    add_compile_options(/W4 /permissive-)
    # Treat warnings as errors in CI; disable locally if desired.
    if(DEFINED ENV{CI})
        add_compile_options(/WX)
    endif()
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(DEFINED ENV{CI})
        add_compile_options(-Werror)
    endif()
endif()

# ─── Shared checker framework library ────────────────────────────────────────
# All problem targets link against this library to get InStream, verdict helpers,
# and graph utilities (which are header-only in graph_utils.hpp).
add_library(checker_core STATIC src/checker_core.cpp)
target_include_directories(checker_core PUBLIC include)

# ─── Problem subdirectories ───────────────────────────────────────────────────
add_subdirectory(problems/shortest_path_certificate)
add_subdirectory(problems/graph_coloring)
add_subdirectory(problems/topological_order)
add_subdirectory(problems/tree_construction)
add_subdirectory(problems/min_cut_certificate)

# ─── CTest integration ────────────────────────────────────────────────────────
# The Python test runner is invoked via CTest so `cmake --build . && ctest` works.
enable_testing()
add_test(
    NAME run_all_tests
    COMMAND ${CMAKE_COMMAND} -E env
        BUILD_DIR=${CMAKE_BINARY_DIR}
        PROBLEMS_DIR=${CMAKE_SOURCE_DIR}/problems
        python3 ${CMAKE_SOURCE_DIR}/scripts/run_all_tests.py
            --build-dir ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
