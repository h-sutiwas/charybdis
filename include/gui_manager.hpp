//
// Created by hamji on 5/4/2026.
//

#ifndef CHARYBDIS_GUI_MANAGER_HPP
#define CHARYBDIS_GUI_MANAGER_HPP

#pragma once

#include <glm/glm.hpp>



struct PhysicsParams {
    float repulsorStrength = 0.025f;
    float contractionFreq = 1.0f;
    float contractionAmp = 0.25f;
    float damping = 0.99f;
    int relaxIterations = 10;
    float shapeMatchingStrength = 0.5f;
    float tentacleGravity = 0.05f;
    float fadeInDuration = 2.0f;
};

struct VisualParams {
    float tubeRadius = 0.02f;
    float fresnelPower = 2.0f;
    float absorptionCoeff = 1.5f;
    float ambientStrength = 0.15f;
    glm::vec3 bodyColor = {0.4f, 0.6f, 0.9f};
    glm::vec3 lightPos = {2.0f, 3.0f, 2.0f};
    glm::vec3 lightColor =  {1.0f, 1.0f, 1.0f};
};

struct CameraParams {
    float azimuth = 0.0f;
    float polar = glm::radians(20.0f);
    float radius = 5.0f;
    float fov = 45.0f;
};


class orbitCamera;

class GuiManager {
public:
    GuiManager(
        PhysicsParams& physics,
        VisualParams& visual,
        CameraParams& camera,
        orbitCamera& orbit_cam
    );

    void render(
        bool& resetRequested,
        bool& wireframe
    );

private:
    PhysicsParams& physics_;
    VisualParams& visual_;
    CameraParams& camera_;
    orbitCamera& orbit_cam_;
};



#endif //CHARYBDIS_GUI_MANAGER_HPP
