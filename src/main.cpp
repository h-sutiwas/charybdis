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

    Shader myShader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");

    // Try Dome Mesh
    Mesh domeMesh = generateDomeMesh(20, 20, 1.0f, 0.7f);

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

    // ImGUI Integration
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

        //
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        myShader.use();

        // Create transformations
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        // model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        // view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        view  = camera.getViewMatrix();
        projection = glm::perspective(glm::radians(45.0f), (float)960 / (float)540, 0.1f, 100.0f);

        // retrieve the matrix uniform locations
        unsigned int modelLoc = glGetUniformLocation(myShader.ID, "model");
        unsigned int viewLoc  = glGetUniformLocation(myShader.ID, "view");

        // Set time uniform
        int timeLoc = glGetUniformLocation(myShader.ID, "time");
        // Set color uniforms
        int col1Loc = glGetUniformLocation(myShader.ID, "color1");
        int col2Loc = glGetUniformLocation(myShader.ID, "color2");

        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        // note: currently we set the projection matrix each frame,
        // but since the projection matrix rarely changes it's often
        // best practice to set it outside the main loop only once.
        myShader.setMat4("projection", projection);

        // Uniform color flickering
        float timeValue = glfwGetTime();

        glUniform1f(timeLoc, timeValue);
        glUniform4f(col1Loc, 1.0f, 0.0f, 0.0f, 1.0f); // Red
        glUniform4f(col2Loc, 0.0f, 0.0f, 1.0f, 1.0f); // blue

        // Draw/Render box
        domeMesh.draw();

        //
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

