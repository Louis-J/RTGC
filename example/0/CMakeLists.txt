CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(Example_0)

# 编译器设置
# setting compiler
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17") # 添加c++17标准支持
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc") # 静态链接
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static") # 静态链接
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall") # 开启额外警告
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g3 -ggdb")  # debug模式下 gdb相关选项
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3") # release模式下 gdb相关选项



# 目录和环境设置
# setting directories
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/../../bin/example") # 可执行文件输出目录
include_directories("${PROJECT_SOURCE_DIR}/../../include") # 头文件包含目录
AUX_SOURCE_DIRECTORY(. DIR_SRCS) # 添加源代码文件夹, 自动扫描所有文件



add_executable(0  # 输出名为NBody的可执行文件
    ${DIR_SRCS}
)
