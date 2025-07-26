cmake_minimum_required(VERSION 3.31.3)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")

project(ws_chat VERSION 1.0 LANGUAGES CXX)

find_package(Boost REQUIRED COMPONENTS system)

add_executable(ws-chat
	src/main.cpp
)

target_include_directories(ws-chat PRIVATE ${CMAKE_SOURCE_DIR}/src)
