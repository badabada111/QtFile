cmake_minimum_required(VERSION 3.10)  #cmake版本最低3.10

project(Develop-Train)  #添加当前项目名称

set(CMAKE_CXX_STANDARD 11) #设置C++标准

#设置 MTD运行库
if(MSVC)     
    # Use the static C library for all build types
    foreach(var 
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
      )
      if(${var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${var} "${${var}}")
      endif()
    endforeach()    
endif(MSVC)


add_subdirectory(thirdParty/googletest)  #添加第三方库的目录

add_subdirectory(GMockExample)
add_subdirectory(GtestExample)

add_subdirectory(PersonCpp)
add_subdirectory(PersonMain)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PersonMain)  #设置启动项



