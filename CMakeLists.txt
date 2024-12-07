cmake_minimum_required(VERSION 3.29)
project(JumpingFrog3)

set(CMAKE_CXX_STANDARD 20)

add_executable(JumpingFrog3 src/main.cpp
        include/config.h
        include/window.h
        src/window.cpp
        include/timer.h
        src/timer.cpp
        include/frog.h
        src/frog.cpp
        include/hole.h
        src/hole.cpp
        include/obstacle.h
        src/obstacle.cpp
        include/friendlyCars.h
        src/friendlyCars.cpp
        include/files.h
        src/files.cpp
        include/stork.h
        src/stork.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ncursesw)
target_link_libraries(${PROJECT_NAME} PRIVATE ncurses -lncurses)