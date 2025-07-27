cmake_minimum_required(VERSION 3.31.3)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")

project(ws_chat VERSION 1.0 LANGUAGES CXX)

add_executable(ws-chat
	src/main.cpp
	src/session_manager.cpp
	src/user.cpp
	src/user_manager.cpp
	src/blocking_queue.hpp
	src/boost_includes.hpp
	src/message.hpp
	src/session_manager.hpp
	src/user.hpp
	src/user_manager.hpp
)

target_include_directories(ws-chat PRIVATE ${CMAKE_SOURCE_DIR}/src)
