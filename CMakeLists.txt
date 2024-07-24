cmake_minimum_required(VERSION 3.20)

project(LibFormat)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(lib-format-bt
	src/common/file.cpp
	src/binary/writer.cpp
	src/binary/reader.cpp
)

set(LIB_FORMAT_SRC ${CMAKE_CURRENT_LIST_DIR}/src)

set_target_properties(lib-format-bt PROPERTIES PREFIX "")
target_include_directories(lib-format-bt PRIVATE ${LIB_FORMAT_SRC})

add_executable(bt src/binary/main.cpp)
target_link_libraries(bt PRIVATE lib-format-bt)
target_include_directories(bt PRIVATE ${LIB_FORMAT_SRC})
