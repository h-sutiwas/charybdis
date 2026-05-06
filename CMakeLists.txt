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
        include/gui_manager.hpp
        src/gui/gui_manager.cpp
)
target_include_directories(glad PUBLIC external/glad/include)

# STB (Interface library - header only library)
add_library(stb INTERFACE
        include/gui_manager.hpp
        src/gui/gui_manager.cpp)
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
        include/gui_manager.hpp
        src/gui/gui_manager.cpp
)

target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(charybdis ${SOURCES}
        src/utils/input_handler.cpp
        include/mesh.hpp
        include/jellyfish_mesh.hpp
        src/mesh/mesh.cpp
        src/mesh/jellyfish_mesh.cpp
        include/particle_system.hpp
        src/simulation/particle_system.cpp)

target_include_directories(charybdis PRIVATE include)

target_link_libraries(charybdis PRIVATE imgui glfw glad glm::glm OpenGL::GL stb)