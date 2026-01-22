cmake_minimum_required(VERSION 3.10)
project(grapedb VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic) 
endif()

file(GLOB_RECURSE LIB_SOURCES "grapedb/*.cpp")
add_library(grape_lib ${LIB_SOURCES})

add_executable(grapedb main.cpp)
target_link_libraries(grapedb PRIVATE grape_lib)

add_executable(grapedb_benchmark benchmark.cpp)
target_link_libraries(grapedb_benchmark PRIVATE grape_lib)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(grapedb_tests tests/test_serializer.cpp tests/test_database.cpp)
target_link_libraries(grapedb_tests PRIVATE gtest_main grape_lib)