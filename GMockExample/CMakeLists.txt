cmake_minimum_required(VERSION 3.10)

project(gmockexample)

set(OBJ ${PROJECT_NAME})

add_executable(${PROJECT_NAME})

#向工程添加源码
target_sources(${OBJ} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/foo.h
)

#向工程添加多个特定头文件的搜索路径——   -I参数
target_include_directories(${OBJ} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/person
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdParty/googletest/googletest/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdParty/googletest/googlemock/include
)

# 链接 Google Test 到测试目标
target_link_libraries(${OBJ} PUBLIC
    gtest 
    gmock
  )

