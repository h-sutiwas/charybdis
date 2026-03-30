//
// Created by hamji on 3/30/2026.
//

#ifndef CHARYBDIS_CAMERA_H
#define CHARYBDIS_CAMERA_H

#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



// Define possible options for camera movement without
enum cameraMovement {
    forward,
    backward,
    left,
    right,
};

// Different type of camera
enum cameraType {
    static_type,
    dynamic_type,
};

// Camera Class
class Camera {
    public:
        glm::vec3 cameraPosition;
        glm::vec3 rotation;
        glm::vec3 scale;
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 model;
        glm::mat4 projection_view;
        glm::mat4 model_view;
        glm::vec3 worldPosition;

};

#endif //CHARYBDIS_CAMERA_H
