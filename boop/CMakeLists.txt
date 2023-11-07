set(CMAKE_CXX_STANDARD 17)

# An asset library
project(boop)

option(USE_ASIMP "Load meshes and scenes with assimp instead of tinyObj" on)

set(SOURCES)
list(APPEND SOURCES
	"boop.h"
	"boop.cpp"
	"common/asset.h"
	"common/asset.cpp"
	"common/texture.h"
	"common/texture.cpp"
	"common/mesh.h"
	"common/mesh.cpp"
	"common/model.h"
	"common/model.cpp"
	"common/material.h"
	"common/material.cpp"
)

if(USE_ASIMP)
	list(APPEND SOURCES 
	"impl/assimpImpl.h"
	"impl/assimpImpl.cpp")
else()
	list(APPEND SOURCES 
	"impl/tinyObjImpl.h"
	"impl/tinyObjImpl.cpp"
	)
endif()

add_library(${PROJECT_NAME} STATIC
	${SOURCES}
)

target_link_libraries(${PROJECT_NAME} PUBLIC vendor)
target_include_directories(${PROJECT_NAME} 
							PUBLIC ${PROJECT_SOURCE_DIR}
							PUBLIC "common")

