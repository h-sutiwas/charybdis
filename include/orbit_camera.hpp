//
// Created by hamji on 5/3/2026.
//

#ifndef CHARYBDIS_CAMERA_HPP
#define CHARYBDIS_CAMERA_HPP

#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class orbitCamera {
public:
    orbitCamera(
        const glm::vec3& center,
        const glm::vec3& upVector,
        float radius,
        float minRadius,
        float maxRadius,
        float azimuthAngle,
        float polarAngle);

    orbitCamera();

    void rotateAzimuth(const float radians);
    void rotatePolar(const float radians);
    void zoom(const float by);

    void moveHorizontal(const float distance);
    void moveVertical(const float distance);

    void setRadius(float newRadius);

    glm::mat4 getViewMatrix() const;
    glm::vec3 getEye() const;
    glm::vec3 getViewPoint() const;
    glm::vec3 getUpVector() const;
    glm::vec3 getNormalizedViewVector() const;

    float getAzimuthAngle() const;
    float getPolarAngle() const;
    float getRadius() const;

private:
    glm::vec3 center_;
    glm::vec3 upVector_;
    float radius_;
    float minRadius_;
    float maxRadius_;
    float azimuthAngle_;
    float polarAngle_;

};


#endif //CHARYBDIS_CAMERA_HPP
