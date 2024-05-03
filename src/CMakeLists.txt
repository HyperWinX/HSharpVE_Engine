include_directories(${HSharpVE_Engine_SOURCE_DIR}/include)

file(GLOB_RECURSE ALL_SRCS ${HSharpVE_Engine_SOURCE_DIR}/src/*.cpp)

add_executable(hve_ng ${ALL_SRCS})
set_target_properties(hve_ng PROPERTIES COMPILE_FLAGS "-Wall -Werror -O0 -ggdb3")