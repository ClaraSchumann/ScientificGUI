cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FILE(GLOB_RECURSE UTIL_SRC src/*.* include/*.hpp)
add_library(UTIL ${UTIL_SRC})
target_include_directories(UTIL PUBLIC include)
target_link_libraries(UTIL PUBLIC rt) # symbol `shm_open`

FILE(GLOB_RECURSE UTIL_TEST_SRC test/*.*)
add_executable(UTIL_TEST ${UTIL_TEST_SRC})
target_link_libraries(UTIL_TEST ${GTEST_LIBRARIES} pthread UTIL)

add_subdirectory(test_multi_process)

