cmake_minimum_required(VERSION 3.28)

project(HSharpVE LANGUAGES CXX C VERSION 0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Compilation flags
set(DEBUG_COMPILE_FLAGS "-Wall -O0 -ggdb3 -fsanitize=address -fsanitize=leak -fsanitize=undefined")
# TODO: fix linker error
set(DEBUG_LINKER_FLAGS "LINKER:-lasan,-fsanitize=address,-fsanitize=leak,-fsanitize=undefined")
set(RELEASE_COMPILE_FLAGS "-Wall -O2 -fdata-sections -ffunction-sections -Wl,--gc-sections")

add_subdirectory(src)

add_executable(hve-ng ${MAIN_SRC})

target_include_directories(hve-ng PUBLIC ${INCLUDE_DIR})
target_link_libraries(hve-ng PRIVATE parser ve)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS ${DEBUG_COMPILE_FLAGS})
    # target_link_options(hve-ng PRIVATE ${DEBUG_LINKER_FLAGS})
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS ${RELEASE_COMPILE_FLAGS})
    add_custom_command(TARGET hve-ng COMMAND POST_BUILD strip -s hve-ng)
endif()
