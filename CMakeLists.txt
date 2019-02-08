project(SDL_SHARED_CONTEXT)
cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BUILD_TYPE Debug)

# set(THREADS_PREFER_PTHREAD_FLAG ON)
# find_package(Threads REQUIRED)

set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/ThirdParty)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/SDLSharedContext)

include_directories(${THIRDPARTY_DIR}/include)

add_executable(shared_context_example ${SOURCE_DIR}/SDLSharedContext.cpp)

target_link_libraries(shared_context_example GLEW SDL2 OpenGL -pthread)
