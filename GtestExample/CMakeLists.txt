cmake_minimum_required(VERSION 3.10)

project(gtestexample)

set(OBJ ${PROJECT_NAME})

add_executable(${PROJECT_NAME})

#向工程添加源码
target_sources(${OBJ} PRIVATE
    test.cpp
)

#向工程添加头文件路径 -I
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../PersonCpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/googletest/googletest/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/googletest/googlemock/include
)

# 链接 Google Test 到测试目标
target_link_libraries(${OBJ} 
    gtest 
    gmock
    PersonCpp
  )
