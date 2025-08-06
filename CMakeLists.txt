cmake_minimum_required(VERSION 3.31)
project(mp3_player)

set(CMAKE_CXX_STANDARD 20)

add_executable(mp3_player main.cpp
        libmp3player.h
        libmp3player.cpp
        source/string.h
        source/Metadata_Handler.cpp
        include/Metadata_Handler.h
        source/Song.cpp
        include/Song.h
        include/JSON_handler.h)
