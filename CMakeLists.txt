cmake_minimum_required(VERSION 3.16)
project(FESH LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_library(fesh STATIC
    src/fesh.cpp
    src/modes.cpp
    src/image.cpp
)


target_include_directories(fesh PUBLIC include)

# 创建可执行程序
add_executable(fesh_cli src/cli.cpp)
target_link_libraries(fesh_cli PRIVATE fesh)
