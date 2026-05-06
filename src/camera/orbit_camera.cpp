//
// Created by hamji on 5/4/2026.
//

#include <glm/glm.hpp>

#include <orbit_camera.hpp>



orbitCamera::orbitCamera(
    const glm::vec3& center,
    const glm::vec3& upVector,
    float radius,
    float minRadius,
    float maxRadius,
    float azimuthAngle,
    float polarAngle)
    : center_(center), upVector_(upVector), radius_(radius), minRadius_(minRadius),
maxRadius_(maxRadius), azimuthAngle_(azimuthAngle), polarAngle_(polarAngle) {
}


void orbitCamera::rotateAzimuth(const float radians) {
    azimuthAngle_ += radians;

    const auto fullCircle = 2.0f*glm::pi<float>();
    azimuthAngle_ = fmodf(azimuthAngle_, fullCircle);
    if (azimuthAngle_ < 0.0f) {
        azimuthAngle_ = fullCircle + azimuthAngle_;
    }
}


void orbitCamera::rotatePolar(const float radians) {
    polarAngle_ += radians;

    const auto polarCap = glm::pi<float>() / 2.0f - 0.001f;
    if (polarAngle_ > polarCap) {
        polarAngle_ = polarCap;
    }

    if (polarAngle_ < -polarCap) {
        polarAngle_ = -polarCap;
    }
}


void orbitCamera::zoom(const float by) {
    radius_ -= by;
    if (radius_ < minRadius_) {
        radius_ = minRadius_;
    } else if (radius_ > maxRadius_) {
        radius_ = maxRadius_;
    }
}


void orbitCamera::setRadius(float newRadius) {
    radius_ = newRadius;
    if (radius_ < minRadius_) {
        radius_ = minRadius_;
    } else if (radius_ > maxRadius_) {
        radius_ = maxRadius_;
    }
}


glm::vec3 orbitCamera::getEye() const {
    // Calculate sines / cosines of angles
    const auto sineAzimuth = glm::sin(azimuthAngle_);
    const auto cosineAzimuth = glm::cos(azimuthAngle_);
    const auto sinePolar = glm::sin(polarAngle_);
    const auto cosinePolar = glm::cos(polarAngle_);

    // Calculate eye position
    const auto x = center_.x + radius_ * cosinePolar * cosineAzimuth;
    const auto y = center_.y + radius_ * sinePolar;
    const auto z = center_.z + radius_ * cosinePolar * sineAzimuth;

    return glm::vec3(x, y, z);
}


void orbitCamera::moveHorizontal(const float distance) {
    const auto position = getEye();
    const glm::vec3 viewVector = getNormalizedViewVector();
    const glm::vec3 strafeVector = glm::normalize(glm::cross(viewVector, upVector_));
    center_ += strafeVector * distance;
}

void orbitCamera::moveVertical(const float distance) {
    center_ += upVector_ * distance;
}


glm::mat4 orbitCamera::getViewMatrix() const {
    return glm::lookAt(getEye(), center_, upVector_);
}


glm::vec3 orbitCamera::getNormalizedViewVector() const {
    return glm::normalize(center_ - getEye());
}


glm::vec3 orbitCamera::getViewPoint() const {
    return center_;
}


glm::vec3 orbitCamera::getUpVector() const {
    return upVector_;
}


float orbitCamera::getRadius() const {
    return radius_;
}


float orbitCamera::getAzimuthAngle() const {
    return azimuthAngle_;
}


float orbitCamera::getPolarAngle() const {
    return polarAngle_;
}