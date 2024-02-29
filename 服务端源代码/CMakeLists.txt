cmake_minimum_required(VERSION 2.8) 

set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++11 -pthread")

# set the project name 
project(proxy) 

set(CMAKE_BUILD_TYPE "Debug")

#把当前目录(.)下所有源代码文件和头文件加入变量SRC_LIST
AUX_SOURCE_DIRECTORY(. SRC_LIST)

# add the executable
add_executable(proxy ${SRC_LIST}) 

