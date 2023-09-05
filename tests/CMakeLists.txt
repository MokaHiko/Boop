cmake_minimum_required(VERSION 3.0)
project(BoopTests)

set(CMAKE_CXX_STANDARD 17)
add_executable(${PROJECT_NAME} tests.cpp importTests.cpp convertTests.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC gtest_main boop vendor)

target_include_directories(${PROJECT_NAME} PUBLIC ../boop)

add_test(NAME ${PROJECT_NAME}
		 COMMAND ${PROJECT_NAME})