set(PROBLEM min_cut)

add_executable(checker_${PROBLEM} checker.cpp)
target_link_libraries(checker_${PROBLEM} PRIVATE checker_core)

add_executable(reference_${PROBLEM} reference.cpp)
target_link_libraries(reference_${PROBLEM} PRIVATE checker_core)

add_executable(generator_${PROBLEM} generator.cpp)
