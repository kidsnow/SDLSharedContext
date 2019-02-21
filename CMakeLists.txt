project(SDL_SHARED_CONTEXT)
cmake_minimum_required(VERSION 3.9)

add_definitions(-DGLEW_NO_GLU -DSDL_MAIN_HANDLED -DSTB_IMAGE_IMPLEMENTATION)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-O0 -Wall -Wextra")

# set(THREADS_PREFER_PTHREAD_FLAG ON)
# find_package(Threads REQUIRED)

set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/ThirdParty)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/SDLSharedContext)

include_directories(${THIRDPARTY_DIR}/include)

file(GLOB SOURCES ${SOURCE_DIR}/*.cpp)

file(COPY ${SOURCE_DIR}/triangle.vert
          ${SOURCE_DIR}/triangle.frag
          ${SOURCE_DIR}/PolygonPlanet.png
          DESTINATION ./)

add_executable(shared_context_example ${SOURCES})

target_link_libraries(shared_context_example GLEW SDL2 OpenGL -pthread)
