//
// Created by hamji on 5/5/2026.
//

#ifndef CHARYBDIS_INPUT_HANDLER_HPP
#define CHARYBDIS_INPUT_HANDLER_HPP

#pragma once

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <orbit_camera.hpp>



void registerCallbacks(GLFWwindow* window);
void processInput(GLFWwindow* window);

#endif //CHARYBDIS_INPUT_HANDLER_HPP
