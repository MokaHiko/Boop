cmake_minimum_required(VERSION 3.0)
project(vendor)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)

add_subdirectory(stb_image)
add_subdirectory(lz4/build/cmake)
add_subdirectory(tinyobjloader)

add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} 
					INTERFACE nlohmann_json::nlohmann_json
					INTERFACE stb_image
					INTERFACE lz4_static
					INTERFACE tinyobjloader
					)

target_include_directories(${PROJECT_NAME} 
					INTERFACE ${PROJECT_SOURCE_DIR}/json/include/nlohmann
					)
