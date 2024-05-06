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
        src/ve/exceptions.cpp)
#Debug target
add_executable(hve_ng-debug ${ALL_SRCS})
set_target_properties(hve_ng-debug PROPERTIES COMPILE_FLAGS "-Wall -O0 -ggdb3")
#Release target
add_executable(hve_ng ${ALL_SRCS}
        src/main/main.cpp
        src/ve/exceptions.cpp)
set_target_properties(hve_ng PROPERTIES COMPILE_FLAGS "-Wall -O2 -fdata-sections -ffunction-sections -Wl,--gc-sections")