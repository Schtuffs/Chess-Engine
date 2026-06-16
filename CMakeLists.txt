cmake_minimum_required(VERSION 3.22)
project(ChessEngine C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC ${CMAKE_CURRENT_LIST_DIR}/src)
set(INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(LIB ${CMAKE_CURRENT_LIST_DIR}/lib)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib
    GIT_TAG        6.0
)

FetchContent_MakeAvailable(raylib)

add_executable(${PROJECT_NAME}
    ${SRC}/Board.cpp
    ${SRC}/Button.cpp
    ${SRC}/main.cpp
    ${SRC}/Piece.cpp
    ${SRC}/Renderer.cpp
    ${SRC}/Utils.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE})
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"
)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++exp)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wpedantic)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
