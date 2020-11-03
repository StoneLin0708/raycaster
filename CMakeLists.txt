cmake_minimum_required(VERSION 3.0.0)

project(raycaster)

find_package(SDL2 REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g")

set(srcs
main.cpp
game.h
game.cpp
raycaster_data.h
raycaster_fixed.h
raycaster_fixed.cpp
raycaster_float.h
raycaster_float.cpp
raycaster_tables.h
raycaster.h
renderer.h
renderer.cpp
)

include_directories(gcem/include)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(raycaster ${srcs})

target_link_libraries(raycaster -lSDL2 -lSDL2_ttf)

file(COPY resource/FreeMono.ttf DESTINATION ${CMAKE_CURRENT_BINARY_DIR})