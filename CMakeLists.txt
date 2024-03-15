cmake_minimum_required(VERSION 3.2)
project(2048)


set(SOURCE_FILES 2048.cpp
        src/game.cpp
        src/game.hpp
)
add_executable(2048 ${SOURCE_FILES})
