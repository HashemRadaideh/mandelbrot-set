cmake_minimum_required(VERSION 3.31.0)

set(NAME mandelbrot)
set(VERSION 0.0.1)

project(${NAME} VERSION ${VERSION} LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB_RECURSE SRC_FILES src/*.c)
add_executable(${NAME} ${SRC_FILES})

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${NAME} raylib m)

add_custom_target(
    run
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -B build/Release -DCMAKE_BUILD_TYPE=Release
    COMMAND ${CMAKE_COMMAND} --build build/Release --config Release
    COMMAND build/Release/${NAME}
    COMMENT "Building and running ${NAME} in Release mode"
)

add_custom_target(
    debug
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -B build/Debug -DCMAKE_BUILD_TYPE=Debug
    COMMAND ${CMAKE_COMMAND} --build build/Debug --config Debug
    COMMENT "Building and running ${NAME} in Debug mode"
)

message(STATUS "CMake build type: ${CMAKE_BUILD_TYPE}")
