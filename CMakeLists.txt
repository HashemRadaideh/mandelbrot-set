cmake_minimum_required(VERSION 3.31.0)

set(NAME mandelbrot)
set(VERSION 0.0.1)

project(${NAME} VERSION ${VERSION} LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
        Release
        CACHE STRING
        "Build type (default: Release)"
        FORCE
    )
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)

file(GLOB_RECURSE SRC_FILES src/*.c)
add_executable(${NAME} ${SRC_FILES})

target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${NAME} raylib m pthread dl)

target_compile_options(
    ${NAME}
    PRIVATE $<$<CONFIG:Release>:-O3> $<$<CONFIG:Debug>:-g>
)

add_custom_target(
    run
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -B ${CMAKE_BINARY_DIR} -DCMAKE_BUILD_TYPE=Release
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config Release
    COMMAND ${CMAKE_BINARY_DIR}/Release/${NAME}
)

add_custom_target(
    debug
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -B .${CMAKE_BINARY_DIR} -DCMAKE_BUILD_TYPE=Debug
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config Debug
    COMMAND gdb ${CMAKE_BINARY_DIR}/Debug/${NAME}
)
