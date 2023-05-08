set(CMAKE_CXX_STANDARD 17)

# An asset library
project(boop)

add_library(${PROJECT_NAME} STATIC
	"boop.h"
	"boop.cpp"
	"common/texture.h"
	"common/texture.cpp"
	"common/asset.h"
	"common/asset.cpp"
)

target_link_libraries(${PROJECT_NAME} PUBLIC vendor)
target_include_directories(${PROJECT_NAME} 
							PUBLIC ${PROJECT_SOURCE_DIR}
							PUBLIC "common")

