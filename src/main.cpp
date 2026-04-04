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
#include <glm/vec3.hpp>

//
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// Vertex Shader
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

// Fragment Shader
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

//
int main(){
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    // Set optional window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(960, 540, "Charybdis", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Make context current
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 960, 540);

    //---- For checking OpenGL version and GPU for rendering ----//
    // std::cout << "OpenGL version:   " << glGetString(GL_VERSION) << std::endl;
    // std::cout << "GPU Vendor:       " << glGetString(GL_VENDOR) << std::endl;
    // std::cout << "GPU Model:        " << glGetString(GL_RENDERER) << std::endl;

    // Vertex Shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Checking for shader compile-time errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Check for shader compile-time errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link Shaders
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check linking failed or not and retrieve corresponding log
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderProgram, 512, nullptr, infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ------ Triangle ------
    float vertices[] = {
        // first triangle
        -0.9f, -0.5f, 0.0f,  // left
        -0.0f, -0.5f, 0.0f,  // right
        -0.45f, 0.5f, 0.0f,  // top
        // second triangle
         0.0f, -0.5f, 0.0f,  // left
         0.9f, -0.5f, 0.0f,  // right
         0.45f, 0.5f, 0.0f   // top
    };

    // Testing a vertex buffer objects to store large number of vertices in the
    // GPU's memory
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Binding the vertex array object first
    glBindVertexArray(VAO);

    // Then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

    // Unbind buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind array
    glBindVertexArray(0);

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Close if click ESC
        processInput(window);

        // Rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}