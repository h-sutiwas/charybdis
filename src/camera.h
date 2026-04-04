//
// Created by hamji on 3/30/2026.
//

#ifndef CHARYBDIS_CAMERA_H
#define CHARYBDIS_CAMERA_H

#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



// Define possible options for camera movement
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
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        glm::vec3 LookAt;
        cameraType type;

        // euler angles
        float Yaw;
        float Pitch;

        // camera controls
        float MovementSpeed;
        float MouseSensitivity;
        float Zoom;
        float lastX;
        float lastY;

        // contructors
        Camera();
        Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

        glm::mat4 GetViewMatrix();


};

#endif //CHARYBDIS_CAMERA_H
