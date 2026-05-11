//
// Created by hamji on 5/6/2026.
//

#ifndef CHARYBDIS_PARTICLE_SYSTEM_HPP
#define CHARYBDIS_PARTICLE_SYSTEM_HPP

#pragma once

#include <vector>
#include <algorithm>
#include <mesh.hpp>
#include <glm/glm.hpp>



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


float contractionWave(float time, float frequency, float powerStrokeFraction);


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

    void integrate(float dt, float domeDamping, float tentacleDamping);

    void solveConstraints(int iterations);

    void applyContraction(
        float time,
        float frequency,
        float amplitude,
        float powerStrokeFraction,
        const std::vector<glm::vec3>& restPositions
        );

    void applyRepulsor(glm::vec3 center, float strength);

    void writeToMesh(Mesh& mesh);

    void reset(const Mesh& originalMesh);

    void applyShapeMatching(const std::vector<glm::vec3>& resetPositions, float strength);

    void syncSeamParticles();

    void generateTentacles(int count, int particlesPerTentacle, float stiffness, int anchorRing = -1);

    void generateInnerTentacles(int count, int particlesPerTentacle, float stiffness, int anchorRing);

    void generateStrandTentacles(int count, int particlesPerTentacle, float stiffness, int anchorRing);

    void applyTentacleGravity(float g);

    void applyTentacleInwardBias(float strength);

    void applyTentacleBuoyancy(float buoyancy);

    void regenerateTentacles(int count, int particlesPerTentacle, float stiffness, int anchorRing = -1);

    void applyApexLift(float strength, float maxLift, const std::vector<glm::vec3>& restPositions);

    std::vector<glm::vec3> getTentaclePositions() const;
    std::vector<glm::vec3> getInnerTentaclePositions() const;
    std::vector<glm::vec3> getStrandTentaclePositions() const;

    int getTentacleCount() const { return tentacleCount_; }
    int getTentacleParticlesPerTentacle() const { return tentacleParticlesPerTentacle_; }
    int getInnerTentacleCount() const { return innerTentacleCount_; }
    int getInnerTentacleParticlesPerTentacle() const { return innerTentacleParticlesPerTentacle_; }
    int getStrandCount() const { return strandCount_; }
    int getStrandParticlesPerTentacle() const { return strandParticlesPerTentacle_; }

private:
    int rings_;
    int slices_;
    int tentacleStartIndex_ = 0;
    int tentacleCount_ = 0;
    int tentacleParticlesPerTentacle_ = 0;
    int tentacleAnchorRing_ = -1;
    std::vector<int> tentacleRimIndices_;
    std::vector<glm::vec3> tentacleInitialPositions_;

    int innerTentacleStartIndex_ = 0;
    int innerTentacleCount_ = 0;
    int innerTentacleParticlesPerTentacle_ = 0;
    int innerTentacleAnchorRing_ = -1;
    std::vector<int> innerTentacleRimIndices_;

    int strandStartIndex_ = 0;
    int strandCount_ = 0;
    int strandParticlesPerTentacle_ = 0;
    int strandAnchorRing_ = -1;
    std::vector<int> strandRimIndices_;
};


#endif //CHARYBDIS_PARTICLE_SYSTEM_HPP
