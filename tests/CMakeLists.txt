cmake_minimum_required(VERSION 3.28)

project(HSharpVE LANGUAGES CXX C VERSION 0.0)

set(TESTS_SRC )
list(APPEND TESTS_SRC tests_tokenizer.cpp)
list(APPEND TESTS_SRC tests_parser.cpp)

add_executable(tests ${TESTS_SRC})

target_link_libraries(tests PRIVATE GTest::gtest_main GTest::gmock)
target_link_libraries(tests PRIVATE parser)