cmake_policy(SET CMP0048 NEW)
cmake_minimum_required(VERSION 3.16)

project(YosupoLibrary)

set(CMAKE_CXX_STANDARD 20)

# TODO: remove mavx2
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
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/f8d7d77c06936315286eb55f8de22cd23c188571.zip
)
FetchContent_MakeAvailable(googletest)

# unittest
add_executable(unittest
  unittest/algebra_test.cpp
  unittest/bit_test.cpp
  unittest/comb_test.cpp 

  unittest/container/binaryheap_test.cpp
  unittest/container/dynamicsegtree_test.cpp
  unittest/container/fastset_test.cpp
  unittest/container/hashmap_test.cpp
  unittest/container/hashset_test.cpp
  unittest/container/segtree_test.cpp
  unittest/container/sparsetable_test.cpp
  unittest/container/splaytree_test.cpp
  unittest/container/vector2d_test.cpp

  unittest/convolution_test.cpp 
  unittest/coord_test.cpp
  unittest/dump_test.cpp
  unittest/fastio_test.cpp  
  unittest/flattenvector_test.cpp
  unittest/fraction_test.cpp
  unittest/hash_test.cpp
  unittest/hl_test.cpp
  unittest/hyperloglog_test.cpp
  unittest/int_n_test.cpp

  unittest/math/basic_test.cpp
  unittest/math/prime_test.cpp
  unittest/math/primitive_root_test.cpp

  unittest/maxflow_test.cpp
  unittest/modint_test.cpp
  unittest/modint8_test.cpp
  unittest/modint61_test.cpp
  unittest/modvec_test.cpp
  unittest/numeric_test.cpp
  unittest/random_test.cpp
  unittest/mst_test.cpp
  unittest/networksimplex_test.cpp
  unittest/toptree_test.cpp
  unittest/util_test.cpp

  unittest/geo/point_test.cpp
  unittest/geo/line_test.cpp
  unittest/geo/polygon_test.cpp
  unittest/geo/convex_test.cpp
  )
target_link_libraries(unittest gtest_main)
add_test(NAME test COMMAND unittest)

# oj
add_executable(oj_aplusb oj/aplusb.test.cpp)
add_executable(oj_factorize oj/factorize.test.cpp)
add_executable(oj_hl_lca oj/hl_lca.test.cpp)
add_executable(oj_matching oj/matching.test.cpp)
add_executable(oj_dynamicseq oj/dynamicseq.test.cpp)
add_executable(oj_mst oj/mst.test.cpp)
add_executable(oj_mcf oj/min_cost_b_flow.test.cpp)
add_executable(oj_staticrmq oj/staticrmq.test.cpp)
