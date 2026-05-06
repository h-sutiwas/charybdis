//
// Created by hamji on 5/6/2026.
//

#ifndef CHARYBDIS_PARTICLE_SYSTEM_HPP
#define CHARYBDIS_PARTICLE_SYSTEM_HPP

#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <mesh.hpp>



struct Particle {
    glm::vec3 position;
    glm::vec3 previousPosition;
    glm::vec3 acceleration;
    bool pinned;    // Top of the dome
};


enum class ConstraintType {
    Horizontal,
    Vertical,
    Diagonal
};

struct Constraint {
    unsigned int indexA;
    unsigned int indexB;
    float restLength;
    float stiffness;
    ConstraintType type;
};


class ParticleSystem {
public:
    std::vector<Particle> particles;
    std::vector<Constraint> constraints;

    void initFromMesh(const Mesh& mesh, int slices);

    void addConstraint(
        unsigned int indexA,
        unsigned int indexB,
        float stiffness,
        ConstraintType type
        );

    void buildConstraints(
        int rings,
        int slices,
        float hoodStiffness,
        float rimStiffness,
        int rimStartRing
        );

    void integrate(float dt, float damping);

    void solveConstraints(int iterations);

    void applyContraction(
        float time,
        float frequency,
        float amplitude,
        const std::vector<glm::vec3>& restPositions
        );

    void applyRepulsor(glm::vec3 center, float strength);

    void writeToMesh(Mesh& mesh);

    void reset(const Mesh& originalMesh);

    void applyShapeMatching(const std::vector<glm::vec3>& resetPositions, float strength);

    void syncSeamParticles();

private:
    int rings_;
    int slices_;
};


#endif //CHARYBDIS_PARTICLE_SYSTEM_HPP
