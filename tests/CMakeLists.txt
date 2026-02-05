cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE TEST_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

list(FILTER TEST_SOURCES EXCLUDE REGEX "src/.*\\.cpp$")

file(GLOB_RECURSE APP_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/**/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/**/*.h"
)

list(FILTER APP_SOURCES EXCLUDE REGEX "main\\.cpp$")

if(TEST_SOURCES)
    add_executable(unit_tests ${TEST_SOURCES} ${APP_SOURCES})
    
    target_include_directories(unit_tests PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${gtest_SOURCE_DIR}/include
    )

    target_include_directories(unit_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${gtest_SOURCE_DIR}/include
    )
    
    target_link_libraries(unit_tests gtest_main)
    
    add_test(NAME unit_tests COMMAND unit_tests)
endif()