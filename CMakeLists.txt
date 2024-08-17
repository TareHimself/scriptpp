cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 20)

project(scriptpp VERSION 1.0.0 LANGUAGES CXX)

option(BUILD_DEV "Build Dev Project" ${PROJECT_IS_TOP_LEVEL})

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cpp" )

file(GLOB_RECURSE INCLUDE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp" )

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${INCLUDE_FILES})

#set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if(MSVC)
 target_compile_options(${PROJECT_NAME} PRIVATE "/MP")
endif()

if(BUILD_DEV)
        add_executable(scriptpp_repl ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
        set_target_properties(scriptpp_repl PROPERTIES OUTPUT_NAME "scriptpp")
        target_link_libraries(scriptpp_repl ${PROJECT_NAME})  
endif()


install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION include
)

install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)

install(
        EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}Config.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME}
)