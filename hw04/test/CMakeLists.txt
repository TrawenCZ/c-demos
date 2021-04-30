
add_executable(unit_test
    runner.cpp
    view.c
# add other test files
)

add_definitions(-DCUT -DTEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(unit_test merger_lib)
include_directories(../src)
