cmake_minimum_required(VERSION 3.28)

project(HSharpVE LANGUAGES CXX C VERSION 0.0)

set(MAIN_SRC src/main/main.cpp PARENT_SCOPE)
list(APPEND MAIN_SRC main/string_split.cpp)

set(PARSER_SRC parser/helpers.cpp)
list(APPEND PARSER_SRC parser/parser.cpp)
list(APPEND PARSER_SRC parser/tokenizer.cpp)

set(VE_SRC ve/ve_main.cpp)
list(APPEND VE_SRC ve/stdlib.cpp)
list(APPEND VE_SRC ve/exceptions.cpp)

add_library(parser STATIC ${PARSER_SRC})
add_library(ve STATIC ${VE_SRC})

target_include_directories(parser PUBLIC ${INCLUDE_DIR})
target_include_directories(ve PUBLIC ${INCLUDE_DIR})
