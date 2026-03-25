//
// Created by h-sutiwas on 2026-03-23.
//

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb/stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <windows.h>

#include <string.h>
#include <vector>
#include <algorithm>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

const int WIDTH = 1920, HEIGHT = 1080;

int main(){
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Charybdis", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    glfwTerminate();

    return 0;
}