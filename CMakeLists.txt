cmake_minimum_required(VERSION 4.1)
project(charybdis)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/dist"
        CACHE PATH "Install prefix" FORCE)
endif()

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

# Find Package
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)

# GLAD
add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# STB (Interface library - header only library)
add_library(stb INTERFACE)
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
)

target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(charybdis ${SOURCES})

target_include_directories(charybdis PRIVATE include)

target_link_libraries(charybdis PRIVATE imgui glfw glad glm::glm OpenGL::GL stb)

include(GNUInstallDirs)

install(TARGETS charybdis
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets/
        DESTINATION ${CMAKE_INSTALL_BINDIR}/assets)

add_custom_command(TARGET charybdis POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:glfw>
                $<TARGET_FILE_DIR:charybdis>
        COMMAND_EXPAND_LISTS
        VERBATIM)

if (MINGW)
    get_filename_component(MINGW_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
    foreach (dll IN ITEMS libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll)
        if (EXISTS ${MINGW_BIN_DIR}/${dll})
            add_custom_command(TARGET charybdis POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            ${MINGW_BIN_DIR}/${dll}
                            $<TARGET_FILE_DIR:charybdis>)
        endif()
    endforeach()
endif()

install(DIRECTORY $<TARGET_FILE_DIR:charybdis>/
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        FILES_MATCHING PATTERN "*.dll")