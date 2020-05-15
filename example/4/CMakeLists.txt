CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(Example_4)

# 编译器设置
# setting compiler
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17") # 添加c++17标准支持
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc") # 静态链接
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static") # 静态链接
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall") # 开启额外警告
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g3 -ggdb")  # debug模式下 gdb相关选项
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -fno-optimize-sibling-calls -fno-tree-tail-merge") # release模式下 gdb相关选项
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -fno-optimize-sibling-calls -ftree-tail-merge") # release模式下 gdb相关选项
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -foptimize-sibling-calls -fno-tree-tail-merge") # release模式下 gdb相关选项
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3") # release模式下 gdb相关选项




# 目录设置
# setting directories
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/../../bin/example") # 可执行文件输出目录
include_directories("${PROJECT_SOURCE_DIR}/../../include") # 头文件包含目录
include_directories("F:/Program/ProMaker/CCPP/Compiler/3rd/include") # Boost
link_directories("F:/Program/ProMaker/CCPP/Compiler/3rd/lib")
AUX_SOURCE_DIRECTORY(. DIR_SRCS) # 添加源代码文件夹, 自动扫描所有文件



# 环境和平台检测
# detecting environment and platform
include(CheckTypeSize)
check_type_size("void *" SIZEOF_VOID_P BUILTIN_TYPES_ONLY)
if (SIZEOF_VOID_P MATCHES "4")
    SET(LIBRARY_PLATFORM_SUFFIX "${CMAKE_CXX_COMPILER_ID}_32")
elseif (SIZEOF_VOID_P MATCHES "8")
    SET(LIBRARY_PLATFORM_SUFFIX "${CMAKE_CXX_COMPILER_ID}_64")
endif ()
message("${LIBRARY_PLATFORM_SUFFIX}")
link_libraries("psapi")
link_libraries("boost_timer-mt_${LIBRARY_PLATFORM_SUFFIX}")
link_libraries("boost_chrono-mt_${LIBRARY_PLATFORM_SUFFIX}")
link_libraries("pthread")



# 其他设置
# setting orther option
# ADD_DEFINITIONS("-DRTGCDEBUG")
# ADD_DEFINITIONS("-DRTGCSPUSE=1")
add_executable(e4  # 输出名为NBody的可执行文件
    ${DIR_SRCS}
)
