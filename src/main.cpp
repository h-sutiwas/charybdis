//
// Created by hamji on 2026-03-23.
//

// #define STB_IMAGE_IMPLEMENTATION
// #include "../external/stb/stb_image.h"

#include <stdio.h>
#include <stdlib.h>
// #include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

// #include <algorithm>
// #include <functional>

#include <filesystem>
// #include <bits/valarray_after.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Handler
#include <input_handler.hpp>

// Shader
#include "shader.hpp"
#include "shader_utils.hpp"

// Camera
#include "orbit_camera.hpp"

// Mesh
#include "jellyfish_mesh.hpp"

// Dear ImGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"



int main() {
    // std::cout << "CWD:" << std::filesystem::current_path().string() << std::endl;

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    // Set optional window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow *window = glfwCreateWindow(960, 540, "Charybdis", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Make context current
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 960, 540);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Camera Default Settings
    orbitCamera camera(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        5.0f,
        1.0f,
        20.0f,
        glm::radians(0.0f),
        glm::radians(20.0f)
    );
    glfwSetWindowUserPointer(window, &camera);

    // Register callback
    registerCallbacks(window);

    // Shader Linking
    Shader myShader(
        "assets/shaders/phong_vertex.glsl",
        "assets/shaders/phong_fragment.glsl"
        );

    // Dome Mesh
    Mesh domeMesh = generateDomeMesh(
        20,
        20,
        1.0f,
        0.7f
        );

    /* Maximum number of vertex attributes we're allowed to declare
    // For OpenGL, there are at least 16 4-component vertex attributes available
    // int nrAttributes;
    // glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    // std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;
    */

    /*---- For checking OpenGL version and GPU for rendering ----
    // std::cout << "OpenGL version:   " << glGetString(GL_VERSION) << std::endl;
    // std::cout << "GPU Vendor:       " << glGetString(GL_VENDOR) << std::endl;
    // std::cout << "GPU Model:        " << glGetString(GL_RENDERER) << std::endl;
    */

    // ImGui Integration
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Close if click ESC
        processInput(window);

        // Rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        myShader.use();

        // Create transformations
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            960.0f/540.0f,
            0.1f,
            100.0f
            );

        // Model-View-Projection uniforms
        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        // Normal matrix (for transforming normals to view space)
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));
        glUniformMatrix3fv(glGetUniformLocation(myShader.ID, "normalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

        // Phong Lighting Uniforms
        glUniform3f(glGetUniformLocation(myShader.ID, "lightPos"), 2.0f, 3.0f, 2.0f);
        glUniform3f(glGetUniformLocation(myShader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(myShader.ID, "objectColor"), 0.4f, 0.6f, 0.9f);

        // Draw/Render box
        domeMesh.draw();

        // ImGui Camera Radius Slider
        ImGui::Begin("Camera Radius");
        ImGui::Text("Adjust Camera Radius");
        float radius = camera.getRadius();
        ImGui::SliderFloat("Radius", &radius, 1.0f, 20.0f);
        camera.setRadius(radius);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Cleanup
    domeMesh.cleanup();
    glDeleteProgram(myShader.ID);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

