set(PROBLEM graph_col)

add_executable(checker_${PROBLEM} checker.cpp)
target_link_libraries(checker_${PROBLEM} PRIVATE checker_core)

add_executable(reference_${PROBLEM} reference.cpp)

add_executable(generator_${PROBLEM} generator.cpp)
