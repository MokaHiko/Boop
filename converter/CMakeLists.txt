project(boop_converter)

set(CMAKE_CXX_STANDARD 17)
add_executable(${PROJECT_NAME} converter.cpp)

target_link_libraries(${PROJECT_NAME} boop vendor)
