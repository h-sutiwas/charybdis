cmake_minimum_required(VERSION 4.1)
project(charybdis)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

# Find Package
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)

# GLAD
add_library(glad STATIC external/glad/src/glad.c
        src/camera.cpp
        src/camera.h)
target_include_directories(glad PUBLIC external/glad/include)

# STB (Interface library - header only library)
add_library(stb INTERFACE
        src/camera.cpp
        src/camera.h)
target_include_directories(stb INTERFACE external/stb)

# Fetch and Build ImGui
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG master
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        src/camera.cpp
        src/camera.h
)

target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(charybdis ${SOURCES}
        src/camera.cpp
        src/camera.h)

target_link_libraries(charybdis PRIVATE imgui glfw glad glm::glm OpenGL::GL stb)