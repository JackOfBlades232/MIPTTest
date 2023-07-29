cmake_minimum_required(VERSION 3.0)
project(game)
find_package(X11 REQUIRED)
set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")
file(GLOB SRC *.cpp)
add_executable(game ${SRC})
target_compile_definitions(game PUBLIC
    $<$<CONFIG:Debug>:USE_ASSERTIONS=1>
    $<$<CONFIG:Debug>:LOG_FPS=1>
    $<$<CONFIG:Release>:USE_ASSERTIONS=0>
    $<$<CONFIG:Release>:LOG_FPS=0>
)
target_link_libraries(game m X11)
