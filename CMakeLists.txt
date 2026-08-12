cmake_minimum_required(VERSION 3.22)
project(ChessEngine C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC     ${CMAKE_CURRENT_LIST_DIR}/src)
set(INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(MOVEGEN ${SRC}/MoveGen)
set(SEARCH  ${SRC}/Search)

add_executable(${PROJECT_NAME}
    ${SRC}/Board.cpp
    ${SRC}/Convert.cpp
    ${SRC}/Engine.cpp
    ${SRC}/Fen.cpp
    ${SRC}/main.cpp
    ${SRC}/Piece.cpp
    ${SRC}/Utils.cpp
    ${SRC}/UCI.cpp
    ${MOVEGEN}/Magic.cpp
    ${MOVEGEN}/MoveGen.cpp
    ${SEARCH}/Search.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${SRC})

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:UTILS_LOG_NONE>)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"
)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++exp)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wpedantic -march=native $<$<CONFIG:Release>:-flto> $<$<COMPILE_LANGUAGE:CXX>:-fconstexpr-ops-limit=1000000000>)

    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
        target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address> $<$<CONFIG:Debug>:-static-libasan>)
    endif()
endif()
