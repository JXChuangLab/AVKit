# 设置项目名称
project(YourProjectName)

# 设置最低的 CMake 版本
cmake_minimum_required(VERSION 3.10)

# 设置 C++ 标准版本为 C++11
set(CMAKE_CXX_STANDARD 11)

# 添加可执行文件
add_executable(flv_parser flv.cpp)