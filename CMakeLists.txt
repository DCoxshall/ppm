cmake_minimum_required(VERSION 3.20)
project(ppm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(external/SDL)

add_executable(${PROJECT_NAME} 
	src/main.cpp
	src/ImageData.h
	src/Parser.cpp
	src/Parser.h
	src/Pixel.h
	src/Scanner.cpp
	src/Scanner.h
	src/Token.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

