cmake_minimum_required(VERSION 3.10)  #cmake版本最低3.10

project(PersonMain)  #添加当前项目名称

set(OBJ ${PROJECT_NAME})

#默认生成静态库
add_executable(${OBJ})

#向工程添加源码
target_sources(${OBJ} PRIVATE
    main.cpp
)

#向工程添加多个特定头文件的搜索路径——   -I参数
target_include_directories(${OBJ} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../PersonCpp
)

target_link_libraries(${OBJ} PRIVATE
    PersonCpp)