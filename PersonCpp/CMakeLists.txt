cmake_minimum_required(VERSION 3.10)  #cmake版本最低3.10

project(PersonCpp)  #添加当前项目名称

set(OBJ ${PROJECT_NAME})

#默认生成静态库
add_library(${OBJ} STATIC)

#向工程添加源码
target_sources(${OBJ} PRIVATE
    person.cpp
    person.h
)

#向工程添加多个特定头文件的搜索路径——   -I参数
target_include_directories(${OBJ} PRIVATE
    ${PROJECT_SOURCE_DIR}
)