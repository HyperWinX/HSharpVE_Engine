cmake_minimum_required(VERSION 3.28)
project(HSharpVE)

set(CMAKE_CXX_STANDARD 23)

#Project setup
include_directories(include)
set(ALL_SRCS
        src/main/main.cpp
        src/parser/helpers.cpp
        src/parser/parser.cpp
        src/parser/tokenizer.cpp
        src/ve/ve_main.cpp
        src/ve/exceptions.cpp
        src/ve/stdlib.cpp)
#Debug target
add_executable(hve_ng-debug ${ALL_SRCS})
set_target_properties(hve_ng-debug PROPERTIES COMPILE_FLAGS "-Wall -O0 -ggdb3 -fsanitize=address -fsanitize=leak -fsanitize=undefined")
target_link_options(hve_ng-debug PUBLIC -lasan -fsanitize=address -fsanitize=leak -fsanitize=undefined)
#Release target
add_executable(hve_ng-release ${ALL_SRCS})
set_target_properties(hve_ng-release PROPERTIES COMPILE_FLAGS "-Wall -O2 -fdata-sections -ffunction-sections -Wl,--gc-sections")
add_custom_command(TARGET hve_ng-release COMMAND POST_BUILD strip -s hve_ng-release)