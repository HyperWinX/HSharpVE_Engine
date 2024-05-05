cmake_minimum_required(VERSION 3.28)
project(HSharpVE)

set(CMAKE_CXX_STANDARD 23)

#Project setup
include_directories(include)
file(GLOB_RECURSE ALL_SRCS ${HSharpVE_Engine_SOURCE_DIR}/src/*.cpp)
#Debug target
add_executable(hve_ng-debug ${ALL_SRCS})
set_target_properties(hve_ng-debug PROPERTIES COMPILE_FLAGS "-Wall -Werror -O0 -ggdb3")
#Release target
add_executable(hve_ng ${ALL_SRCS}
        src/main/main.cpp)
set_target_properties(hve_ng PROPERTIES COMPILE_FLAGS "-Wall -Werror -O2 -fdata-sections -ffunction-sections -Wl,--gc-sections")