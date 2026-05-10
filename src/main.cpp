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
#include "input_handler.hpp"
#include "gui_manager.hpp"

// Shader
#include "shader.hpp"
#include "shader_utils.hpp"

// Camera
#include "orbit_camera.hpp"

// Mesh
#include "jellyfish_mesh.hpp"

// Physics Simulation
#include "particle_system.hpp"

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
    GLFWwindow *window = glfwCreateWindow(
        960,
        540,
        "Charybdis",
        nullptr,
        nullptr
        );
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
    glEnable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    PhysicsParams physicsParams;
    VisualParams visualParams;
    CameraParams cameraParams;
    bool wireframe = false;
    bool resetSim = false;

    // Camera Default Settings
    orbitCamera camera(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        cameraParams.radius,
        1.0f, 20.0f,
        cameraParams.azimuth,
        cameraParams.polar
    );
    glfwSetWindowUserPointer(window, &camera);

    // Register callback
    registerCallbacks(window);

    // Shader Linking
    Shader myShader(
        "assets/shaders/phong_vertex.glsl",
        "assets/shaders/jelly_fragment.glsl"
        );
    Shader tentacleShader(
        "assets/shaders/tentacle_vertex.glsl",
        "assets/shaders/tentacle_geometry.glsl",
        "assets/shaders/tentacle_fragment.glsl"
    );

    // Dome Mesh
    Mesh domeMesh = generateDomeMesh(
        30,
        30,
        1.5f,
        0.4f
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

    // Particle System
    ParticleSystem particleSystem;
    particleSystem.initFromMesh(domeMesh, 30);
    particleSystem.buildConstraints(
        30,
        30,
        0.8f,
        0.3f,
        24
        );

    // ImGui Integration
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    GuiManager guiManager(physicsParams, visualParams, cameraParams, camera);

    Mesh originalMesh = domeMesh;
    std::vector<glm::vec3> resetPositions;
    for (const auto& v : domeMesh.vertices) {
        resetPositions.push_back(v.Position);
    }

    // Tentacles
    const int tentacleCount = 12;
    const int tentacleParticles = 10;
    particleSystem.generateTentacles(tentacleCount, tentacleParticles, 0.15f);

    GLuint tentacleVAO, tentacleVBO;
    glGenVertexArrays(1, &tentacleVAO);
    glGenBuffers(1, &tentacleVBO);
    glBindVertexArray(tentacleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tentacleVBO);

    int tentaclePosCount = tentacleCount * (tentacleParticles + 1);
    glBufferData(GL_ARRAY_BUFFER, tentaclePosCount * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Close if click ESC
        processInput(window);

        // Rendering commands
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create transformations
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(
            glm::radians(cameraParams.fov),
            960.0f / 540.0f,
            0.1f, 100.0f
            );

        // Normal matrix (for transforming normals to view space)
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

        // Model-View-Projection uniforms
        myShader.use();
        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        glUniformMatrix3fv(glGetUniformLocation(myShader.ID, "normalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

        // Phong Lighting Uniforms
        glUniform3f(glGetUniformLocation(myShader.ID, "lightPos"), visualParams.lightPos[0], visualParams.lightPos[1], visualParams.lightPos[2]);
        glUniform3f(glGetUniformLocation(myShader.ID, "lightColor"), visualParams.lightColor[0], visualParams.lightColor[1], visualParams.lightColor[2]);
        glUniform3f(glGetUniformLocation(myShader.ID, "objectColor"), visualParams.bodyColor[0], visualParams.bodyColor[1], visualParams.bodyColor[2]);
        myShader.setFloat("fresnelPower", visualParams.fresnelPower);
        myShader.setFloat("absorptionCoeff", visualParams.absorptionCoeff);
        myShader.setFloat("ambientStrength", visualParams.ambientStrength);

        // Eye View
        glUniform3fv(glGetUniformLocation(myShader.ID, "viewPos"), 1, glm::value_ptr(camera.getEye()));

        float elapsed = glfwGetTime();
        float fadeIn = glm::min(elapsed / physicsParams.fadeInDuration, 1.0f);

        // Physics Update
        particleSystem.applyRepulsor(
            glm::vec3(0.0f, 0.2f, 0.0f),
            physicsParams.repulsorStrength
        );
        particleSystem.applyContraction(
            glfwGetTime(),
            physicsParams.contractionFreq,
            physicsParams.contractionAmp * fadeIn,
            resetPositions
        );
        particleSystem.applyShapeMatching(resetPositions, physicsParams.shapeMatchingStrength);
        particleSystem.integrate(
            1.0f / 60.0f,
            physicsParams.damping
        );
        particleSystem.solveConstraints(physicsParams.relaxIterations);
        particleSystem.writeToMesh(domeMesh);
        domeMesh.updateBuffers();

        // Draw/Render box
        domeMesh.draw();

        // Draw tentacles
        auto tentPos = particleSystem.getTentaclePositions();
        glBindBuffer(GL_ARRAY_BUFFER, tentacleVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(tentPos.size() * sizeof(glm::vec3)), tentPos.data());

        tentacleShader.use();
        tentacleShader.setMat4("model", model);
        tentacleShader.setMat4("view", view);
        tentacleShader.setMat4("projection", projection);

        tentacleShader.setFloat("tubeRadius", visualParams.tubeRadius);

        glUniform3f(glGetUniformLocation(tentacleShader.ID, "tentacleColor"), visualParams.bodyColor[0], visualParams.bodyColor[1], visualParams.bodyColor[2]);

        glBindVertexArray(tentacleVAO);
        for (int t = 0; t < tentacleCount; ++t) {
            glDrawArrays(GL_LINE_STRIP, t * (tentacleParticles + 1), tentacleParticles + 1);
        }
        glBindVertexArray(0);

        wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        if (resetSim) particleSystem.reset(originalMesh);

        guiManager.render(
            resetSim,
            wireframe
        );

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
    glDeleteVertexArrays(1, &tentacleVAO);
    glDeleteBuffers(1, &tentacleVBO);
    glDeleteProgram(myShader.ID);
    glDeleteProgram(tentacleShader.ID);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

