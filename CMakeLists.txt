cmake_minimum_required(VERSION 3.20)

project(BinaryTree)

add_executable(main
	src/main.cpp
	src/file.cpp
	src/reader.cpp
	src/writer.cpp
)

target_include_directories(main PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/src
)
