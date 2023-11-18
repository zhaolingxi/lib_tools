cmake_minimum_required (VERSION 3.1)
project(lib_tools)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_COMPILER /path/to/g++)
#set(CMAKE_C_COMPILER /path/to/gcc)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# main src
file(GLOB TOOL_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/code/source/*.cpp
    )

add_executable(${PROJECT_NAME}
"${TOOL_SRC}"
)