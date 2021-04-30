
add_library(merger_lib STATIC
# add here file you want to either use in your solution and also you want to test them
    ini_parser.c
)
add_executable(merge-ini main.c)

target_link_libraries(merge-ini merger_lib)