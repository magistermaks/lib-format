cmake_minimum_required(VERSION 3.20)

project(LibFormat)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(lib-format-bt
	src/common/file.cpp
	src/binary/writer.cpp
	src/binary/reader.cpp
)

add_library(lib-format-tt
	src/common/file.cpp
	src/text/token.cpp
	src/text/span.cpp
	src/text/error.cpp
	src/text/parser.cpp
)

set(LIB_FORMAT_SRC ${CMAKE_CURRENT_LIST_DIR}/src)

# Configure
set_target_properties(lib-format-bt PROPERTIES PREFIX "")
target_include_directories(lib-format-bt PRIVATE ${LIB_FORMAT_SRC})
set_target_properties(lib-format-tt PROPERTIES PREFIX "")
target_include_directories(lib-format-tt PRIVATE ${LIB_FORMAT_SRC})

# The BT utility
add_executable(bt
	src/binary/main.cpp
	src/common/util.cpp
)
target_link_libraries(bt PRIVATE lib-format-bt)
target_include_directories(bt PRIVATE ${LIB_FORMAT_SRC})

# The TT utility
add_executable(tt
	src/text/main.cpp
	src/common/util.cpp
)
target_link_libraries(tt PRIVATE lib-format-tt)
target_include_directories(tt PRIVATE ${LIB_FORMAT_SRC})

# You can then install with 'sudo make install'
install(TARGETS bt)
install(TARGETS tt)
