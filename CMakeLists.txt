cmake_minimum_required(VERSION 3.29)
project(JumpingFrog2)

set(CMAKE_CXX_STANDARD 20)

add_executable(JumpingFrog2 main.cpp
        config.h)

target_link_libraries(${PROJECT_NAME} PRIVATE ncursesw)
target_link_libraries(${PROJECT_NAME} PRIVATE ncurses -lncurses)