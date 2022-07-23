cmake_minimum_required(VERSION 3.10)

project(JABG)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
find_package(Freetype REQUIRED)
find_package(glm REQUIRED)
find_package(Threads)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

#is it compiled on normie platform?
if(MSVC)
    set(CMAKE_CXX_FLAGS "/O12x /EHsc /experimental:module /std:c++latest /I")
else()
    set(CMAKE_C_COMPILER /usr/bin/gcc)
    set(CMAKE_CXX_COMPILER /usr/bin/gcc)
    set(CMAKE_CXX_FLAGS "-ggdb -Wall -Wno-reorder -Wno-sign-compare -Wno-unused-command-line-argument -march=native ")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()


file(GLOB_RECURSE SOURCES   
            ${CMAKE_SOURCE_DIR}/src/*.cpp 
            ${CMAKE_SOURCE_DIR}/glad.c 
            ${CMAKE_SOURCE_DIR}/main.cpp
)

add_executable(JABG ${SOURCES})

target_link_libraries(JABG ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES} ${GLM_LIBRARIES} m stdc++ ${CMAKE_DL_LIBS})