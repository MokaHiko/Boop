cmake_minimum_required(VERSION 3.0)
project(vendor)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)

add_subdirectory(stb_image)
add_subdirectory(lz4/build/cmake)
add_subdirectory(tinyobjloader)

option(BP_BUILD_GLM "False if glm already exists in rpoject" FALSE)

if(BP_BUILD_GLM)
	add_subdirectory(glm)
endif()

add_subdirectory(assimp)

set(gtest_force_shared_crt on)
add_subdirectory(gtest)

add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} 
					INTERFACE nlohmann_json::nlohmann_json
					INTERFACE stb_image
					INTERFACE lz4_static
					INTERFACE tinyobjloader
					INTERFACE assimp
					INTERFACE glm
					)

target_include_directories(${PROJECT_NAME} 
					INTERFACE ${PROJECT_SOURCE_DIR}/json/include/nlohmann
					)
