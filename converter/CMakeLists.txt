project(boop_converter)

set(CMAKE_CXX_STANDARD 17)
add_executable(${PROJECT_NAME} converter.cpp)

set_target_properties(${PROJECT_NAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(${PROJECT_NAME} boop vendor)
