//
// Created by hamji on 5/5/2026.
//

#include <input_handler.hpp>



bool leftMouseButtonDown = false;
bool wireframe = false;
double lastMouseX = 0.0;
double lastMouseY = 0.0;
double sensitivity = 0.005f;


void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}


void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (ImGui::GetIO().WantCaptureMouse) return;
    auto* camera = static_cast<orbitCamera*>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        leftMouseButtonDown = true;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        leftMouseButtonDown = false;
    }

    glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
}


void cursor_pos_callback(GLFWwindow* window, double xPos, double yPos) {
    if (ImGui::GetIO().WantCaptureMouse) return;
    auto* camera = static_cast<orbitCamera*>(glfwGetWindowUserPointer(window));
    if (!leftMouseButtonDown) return;

    float xOffset = xPos - lastMouseX;
    float yOffset = yPos - lastMouseY;

    lastMouseX = xPos;
    lastMouseY = yPos;

    camera->rotateAzimuth(xOffset * sensitivity);
    camera->rotatePolar(yOffset * sensitivity);
};


void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
    auto* camera = static_cast<orbitCamera*>(glfwGetWindowUserPointer(window));
    camera->zoom(yOffset);
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        wireframe = !wireframe;
        if (wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
}


void registerCallbacks(GLFWwindow* window) {
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
}