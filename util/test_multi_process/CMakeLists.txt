cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB TASK_SOURCES  ./*.cpp )
foreach(testsourcefile ${TASK_SOURCES} )
    cmake_path(GET testsourcefile STEM testname)
    add_executable( ${testname} ${testsourcefile})
    target_link_libraries( ${testname} UTIL)
endforeach( testsourcefile ${TASK_SOURCES} )
