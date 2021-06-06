cmake_policy(SET CMP0048 NEW)
project(YosupoLibrary)

cmake_minimum_required(VERSION 3.16)

if(NOT "${CMAKE_CXX_STANDARD}")
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT MSVC)
    message("Not MSVC")
    add_compile_options(-Wall -Wextra -Wshadow -Wconversion -Wno-sign-conversion -Werror)
    add_compile_options(-fsanitize=undefined,address -fno-sanitize-recover=all)
    add_compile_options(-march=native)
    add_link_options(-fsanitize=undefined,address)
else()
    message("Use MSVC")
    add_compile_options(/W4 /permissive- /sdl)
endif()

enable_testing()

add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include(GoogleTest)

include_directories(.)
include_directories(../../)
include_directories(../ac-library)

add_executable(BitTest bit_test.cpp)
target_link_libraries(BitTest gtest gtest_main)
gtest_discover_tests(BitTest)

add_executable(FastSetTest fastset_test.cpp)
target_link_libraries(FastSetTest gtest gtest_main)
gtest_discover_tests(FastSetTest)

add_executable(HashTest hash_test.cpp)
target_link_libraries(HashTest gtest gtest_main)
gtest_discover_tests(HashTest)

add_executable(HashMapTest hashmap_test.cpp)
target_link_libraries(HashMapTest gtest gtest_main)
gtest_discover_tests(HashMapTest)


add_executable(RandomTest random_test.cpp)
target_link_libraries(RandomTest gtest gtest_main)
gtest_discover_tests(RandomTest)

add_executable(FastIOTest fastio_test.cpp)
target_link_libraries(FastIOTest gtest gtest_main)
gtest_discover_tests(FastIOTest)

add_executable(MathTest math_test.cpp)
target_link_libraries(MathTest gtest gtest_main)
gtest_discover_tests(MathTest)

add_executable(ModIntTest modint_test.cpp)
target_link_libraries(ModIntTest gtest gtest_main)
gtest_discover_tests(ModIntTest)
