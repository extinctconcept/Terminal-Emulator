cmake_minimum_required (VERSION 3.0)
project (Assign4)

if (CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-psabi -g3 -std=c++14 ${CMAKE_CXX_FLAGS}")
endif()

file (GLOB CPP src/*.cpp)

set (SOURCE_FILES ${CPP})

add_executable(Assign4 ${SOURCE_FILES})
