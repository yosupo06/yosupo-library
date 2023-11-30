cmake_policy(SET CMP0048 NEW)
cmake_minimum_required(VERSION 3.16)

project(YosupoLibrary)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wshadow -Wconversion -Wno-sign-conversion -Werror)

# sanitizer
add_compile_options("$<$<CONFIG:DEBUG>:-fsanitize=undefined,address;-fno-sanitize-recover=all>")
add_link_options("$<$<CONFIG:DEBUG>:-fsanitize=undefined,address>")

include_directories(../src)
include_directories(../ac-library)
include_directories(./util)

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/f8d7d77c06936315286eb55f8de22cd23c188571.zip
)
FetchContent_MakeAvailable(googletest)

# unittest
add_executable(unittest
  unittest/binaryheap_test.cpp)
target_link_libraries(unittest gtest_main)
add_test(NAME test COMMAND unittest)
