cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 20)

project(time VERSION 1.0.0 LANGUAGES CXX)

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cpp" )

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${INCLUDE_FILES})
# Gives our library file a .node extension without any "lib" prefix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".sppn")

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../build/Debug)

find_library(scriptpp_LIB scriptpp)

target_link_libraries(${PROJECT_NAME} scriptpp::scriptpp)
