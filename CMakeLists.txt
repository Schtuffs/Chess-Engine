cmake_minimum_required(VERSION 3.22)
project(ChessEngine C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC ${CMAKE_CURRENT_LIST_DIR}/src)
set(INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)

add_executable(${PROJECT_NAME}
    ${SRC}/Board.cpp
    ${SRC}/Engine.cpp
    ${SRC}/main.cpp
    ${SRC}/Piece.cpp
    ${SRC}/Utils.cpp
    ${SRC}/UCI.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE})
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"
)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++exp)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wpedantic)
endif()
