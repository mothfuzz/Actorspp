cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")

project(actorspp)

add_executable(${PROJECT_NAME} src/main.cpp)
include_directories(${PROJECT_NAME} include/)
include_directories(${PROJECT_NAME} include/renderer/)
include_directories(${PROJECT_NAME} include/py/)
include_directories(${PROJECT_NAME} src/actors/)
include_directories(${PROJECT_NAME} src/tests/)

#sdl2
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)
find_package(sdl2-image CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2_image)
#gl
find_package(glad CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)
find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
#python
#find_package(Python3 COMPONENTS Development REQUIRED)
#target_link_libraries(${PROJECT_NAME} PRIVATE Python3::Python)
find_package(pybind11 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::embed)
