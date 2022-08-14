cmake_minimum_required(VERSION 3.10)

project(JABG)

set(CMAKE_CXX_STANDARD 17)
set(SDL2W_SDL2_DIR ${CMAKE_SOURCE_DIR}/libs/SDL2)

add_subdirectory(libs/glad)
add_subdirectory(libs/glm)

find_package(SDL2 REQUIRED)
find_package(Freetype REQUIRED)
find_package(Threads)
find_package(ZLIB)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/libs)

#is it compiled on normie platform?
if(MSVC)
    set(CMAKE_CXX_FLAGS "/O12x /EHsc /experimental:module /std:c++latest /I")
else()
    set(CMAKE_C_COMPILER /usr/bin/gcc)
    set(CMAKE_CXX_COMPILER /usr/bin/gcc)
    set(CMAKE_CXX_FLAGS " -ggdb -Wall -Wno-reorder -Wno-sign-compare -Wno-unused-command-line-argument -march=native ")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()


file(GLOB_RECURSE SOURCES   
            ${CMAKE_SOURCE_DIR}/src/*.cpp 
            ${CMAKE_SOURCE_DIR}/glad.c 
            ${CMAKE_SOURCE_DIR}/main.cpp
)

add_executable(JABG ${SOURCES})

target_link_libraries(JABG glm glad ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES} ${GLM_LIBRARIES} m stdc++   ${CMAKE_DL_LIBS} ZLIB::ZLIB)