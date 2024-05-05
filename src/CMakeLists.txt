include_directories(${HSharpVE_Engine_SOURCE_DIR}/include)

set(ALL_SRCS src/main/main.cpp
             src/parser/parser.cpp
             src/parser/tokenizer.cpp
             src/parser/helpers.cpp
             src/ve/ve_functions.cpp
             src/ve/ve_main.cpp)

add_executable(hve_ng ${ALL_SRCS})
set_target_properties(hve_ng PROPERTIES COMPILE_FLAGS "-Wall -Werror -O0 -ggdb3")
add_custom_command(TARGET hve_ng POST_BUILD COMMAND "hve_ng ../test.hv")