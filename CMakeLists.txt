cmake_minimum_required(VERSION 3.10)
project(task LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp")

add_executable(task ${SOURCES})

target_include_directories(task PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/release-1.12.1.zip
    )
    FetchContent_MakeAvailable(googletest)
    
    add_subdirectory(tests)
endif()