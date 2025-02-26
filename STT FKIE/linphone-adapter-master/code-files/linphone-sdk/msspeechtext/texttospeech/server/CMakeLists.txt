cmake_minimum_required(VERSION 3.2)
project(texttospeechServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pg -g3")

message(STATUS "Building project: ${PROJECT_NAME}")

# Include external dependencies
include_directories(${CMAKE_BINARY_DIR}/external/include)
link_directories(${CMAKE_BINARY_DIR}/external/lib)

# Include directories for this project
include_directories(include)
include_directories(model)
include_directories(api)
include_directories(impl)

# Collect source files
file(GLOB SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/api/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/model/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# Define the library as SHARED
add_library(${PROJECT_NAME} SHARED ${SRCS})
add_dependencies(${PROJECT_NAME} NLOHMANN)
target_link_libraries(${PROJECT_NAME} PRIVATE pistache pthread)

# Ensure correct include directories
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/msspeechtext/texttospeech/server/include>
        $<INSTALL_INTERFACE:include/texttospeech>
)

# INSTALL the library properly
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)

# EXPORT the target (for build-tree and install-tree)
install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME}
)

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION include/texttospeech
        FILES_MATCHING PATTERN "*.h"
)
