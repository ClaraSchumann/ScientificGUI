cmake_minimum_required(VERSION 3.5)

project(SCIGUI LANGUAGES CXX)

include_directories(${GTEST_INCLUDE_DIRS})
find_package(GTest REQUIRED)

add_subdirectory(util)
