//
// Created by hamji on 5/6/2026.
//

#include "particle_system.hpp"

#include <cmath>
#include <set>
#include <map>
#include <glm/gtc/constants.hpp>


float contractionWave(float time, float frequency, float powerStrokeFraction) {
    float phase = time * frequency;
    phase = phase - std::floor(phase);
    const float kRecoveryDip = 0.3f;
    if (phase < powerStrokeFraction) {
        float local = phase / powerStrokeFraction;
        return std::sin(local * glm::pi<float>());
    } else {
        float local = (phase - powerStrokeFraction) / (1.0f - powerStrokeFraction);
        return -kRecoveryDip * std::sin(local * glm::pi<float>());
    }
}


static float tentacleLengthScale(int t) {
    float s = std::sin(static_cast<float>(t) * 12.9898f) * 43758.5453f;
    float h = s - std::floor(s);
    return 0.8f + 0.4f * h;
}


void ParticleSystem::initFromMesh(const Mesh &mesh, int slices) {
    particles.clear();
    for (const auto& vertex : mesh.vertices) {
        Particle p;
        p.position = vertex.Position;
        p.previousPosition = vertex.Position;
        p.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
        p.pinned = false;
        particles.push_back(p);
    }
    for (int slice = 0; slice <= slices; ++slice) {
        particles[slice].pinned = true;
    }
}


void ParticleSystem::addConstraint(unsigned int indexA, unsigned int indexB, float stiffness, ConstraintType type) {
    float restLength = glm::length(particles[indexB].position - particles[indexA].position);
    constraints.push_back(Constraint(indexA, indexB, restLength, stiffness, type));
}


void ParticleSystem::buildConstraints(int rings, int slices, float hoodStiffness, float rimStiffness, int rimStartRing) {
    rings_ = rings;
    slices_ = slices;

    std::map<std::pair<int, int>, ConstraintType> constraintMap;

    for (int ring = 0; ring <= rings; ++ring) {
        for (int slice = 0; slice <= slices; ++slice) {
            int current = ring * (slices + 1) + slice;

            int nextSlice = ring * (slices + 1) + (slice + 1);
            int nextRing = (ring + 1) * (slices + 1) + slice;
            int nextDiagonal = (ring + 1) * (slices + 1) + (slice + 1);

            // float stiffness = (ring >= rimStartRing) ? rimStiffness : hoodStiffness;

            if (slice + 1 <= slices) {
		        int hTarget = (slice + 1 == slices) ? ring * (slices + 1) : nextSlice;
                auto key = std::make_pair(std::min(current, hTarget), std::max(current, hTarget));
                constraintMap[key] = ConstraintType::Horizontal;
            }

            if (ring + 1 <= rings) {
                auto key = std::make_pair(std::min(current, nextRing), std::max(current, nextRing));
                constraintMap[key] = ConstraintType::Vertical;
            }

            if (ring + 1 <= rings && slice + 1 <= slices) {
                int dTarget = (slice + 1 == slices) ? (ring + 1) * (slices + 1) : nextDiagonal;
                auto key = std::make_pair(std::min(current, dTarget), std::max(current, dTarget));
                constraintMap[key] = ConstraintType::Diagonal;
            }
        }
    }

    for (const auto& [edge, type] : constraintMap) {
        int ringA = edge.first / (slices + 1);
        int ringB = edge.second / (slices + 1);
        float stiffness = (ringA >= rimStartRing || ringB >= rimStartRing) ? rimStiffness : hoodStiffness;
        addConstraint(edge.first, edge.second, stiffness, type);
    }
}


void ParticleSystem::integrate(float dt, float domeDamping, float tentacleDamping) {
    for (size_t i = 0; i < particles.size(); ++i) {
        auto& p = particles[i];
        if (p.pinned) continue;

        glm::vec3 velocity = p.position - p.previousPosition;
        p.previousPosition = p.position;

        float d = (tentacleStartIndex_ > 0 && i >= static_cast<size_t>(tentacleStartIndex_))
            ? tentacleDamping
            : domeDamping;

        p.position = p.position + (velocity * d) + (p.acceleration * dt * dt);

        p.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    }
}


void ParticleSystem::solveConstraints(int iterations) {
    for (int iteration = 0; iteration < iterations; ++iteration) {
        for (auto& c : constraints) {
            Particle& a = particles[c.indexA];
            Particle& b = particles[c.indexB];

            glm::vec3 delta = b.position - a.position;
            float currentLength = glm::length(delta);

            if (currentLength < 1e-6f) continue;

            float error = (currentLength - c.restLength) / currentLength;
            glm::vec3 correction = delta * error * 0.5f * c.stiffness;

            if (!a.pinned) a.position += correction;
            if (!b.pinned) b.position -= correction;
        }
        syncSeamParticles();
    }
}


void ParticleSystem::applyContraction(float time, float frequency, float amplitude, float powerStrokeFraction, const std::vector<glm::vec3>& restPositions) {
    float wave = contractionWave(time, frequency, powerStrokeFraction);
    glm::vec3 center = glm::vec3(0.0f, 0.4f, 0.0f);  // center of bell interior

    for (size_t i = 0; i < restPositions.size(); ++i) {
        if (particles[i].pinned) continue;

        // Use Rest position for stable direction
        glm::vec3 toCenter = center - restPositions[i];
        float dist = glm::length(toCenter);
        if (dist < 1e-6f) continue;

        float heightFactor = 1.0f - glm::clamp(restPositions[i].y / 1.4f, 0.0f, 1.0f);
        float strength = amplitude * wave * heightFactor;
        particles[i].acceleration += (toCenter / dist) * strength;
    }
}


void ParticleSystem::applyRepulsor(glm::vec3 center, float strength) {
    int domeCount = (tentacleStartIndex_ > 0) ? tentacleStartIndex_ : static_cast<int>(particles.size());

    for (int i = 0; i < domeCount; ++i) {
        Particle& p = particles[i];
        if (p.pinned) continue;
        glm::vec3 direction = p.position - center;
        float distSq = glm::max(glm::dot(direction, direction), 0.01f);
        if (distSq > 1e-6f) {
            float distance = glm::sqrt(distSq);
            p.acceleration += (direction / distance) * (strength / distSq);
        }
    }
}


void ParticleSystem::writeToMesh(Mesh& mesh) {
    for (size_t i = 0; i < mesh.vertices.size(); ++i) {
        mesh.vertices[i].Position = particles[i].position;
    }

    for (int ring = 0; ring <= rings_; ++ring) {
        int firstInRing = ring * (slices_ + 1);
        int lastInRing = firstInRing + slices_;
        particles[lastInRing].position = particles[firstInRing].position;
        mesh.vertices[lastInRing].Position = mesh.vertices[firstInRing].Position;
    }
}


void ParticleSystem::reset(const Mesh& originalMesh) {
    for (size_t i = 0; i < originalMesh.vertices.size(); ++i) {
        particles[i].position = originalMesh.vertices[i].Position;
        particles[i].previousPosition = originalMesh.vertices[i].Position;
        particles[i].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    for (size_t k = 0; k < tentacleInitialPositions_.size(); ++k) {
        size_t i = tentacleStartIndex_ + k;
        if (i >= particles.size()) break;
        particles[i].position = tentacleInitialPositions_[k];
        particles[i].previousPosition = tentacleInitialPositions_[k];
        particles[i].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    }
}


void ParticleSystem::applyShapeMatching(const std::vector<glm::vec3>& resetPositions, float strength) {
    for (size_t i = 0; i < resetPositions.size(); ++i) {
        if (particles[i].pinned) continue;
        glm::vec3 toReset = resetPositions[i] - particles[i].position;
        particles[i].acceleration += toReset * strength;
    }
}


void ParticleSystem::syncSeamParticles() {
    for (int ring = 0; ring <= rings_; ++ring) {
        int first = ring * (slices_ + 1);
        int last = first + slices_;
        particles[last].position = particles[first].position;
        particles[last].previousPosition = particles[first].previousPosition;
    }
}


void ParticleSystem::generateTentacles(int count, int particlesPerTentacle, float stiffness, int anchorRing) {
    tentacleStartIndex_ = static_cast<int>(particles.size());
    tentacleCount_ = count;
    tentacleParticlesPerTentacle_ = particlesPerTentacle;
    tentacleRimIndices_.clear();
    tentacleInitialPositions_.clear();

    const float spacing = 0.1f;
    int ring = (anchorRing >= 0) ? anchorRing : (rings_ - 10);
    if (ring < 1) ring = 1;
    if (ring > rings_) ring = rings_;
    tentacleAnchorRing_ = ring;

    for (int t = 0; t < count; ++t) {
	    int slice = static_cast<int>(std::round(static_cast<float>(t) * static_cast<float>(slices_) / static_cast<float>(count))) % slices_;
	    int rimIndex = tentacleAnchorRing_ * (slices_ + 1) + slice;
	    tentacleRimIndices_.push_back(rimIndex);
	    glm::vec3 rootPos = particles[rimIndex].position;

	    float thisSpacing = spacing * tentacleLengthScale(t * 7919 + 13);

	    for (int p = 0; p < particlesPerTentacle; ++p) {
	        Particle particle{};
	        particle.position = rootPos + glm::vec3(0.0f, -static_cast<float>(p + 1) * thisSpacing, 0.0f);
	        particle.previousPosition = particle.position;
	        particle.acceleration = glm::vec3(0.0f);
	        particle.pinned = false;
	        tentacleInitialPositions_.push_back(particle.position);
	        particles.push_back(particle);
	    }

	    addConstraint(
	        rimIndex,
	        tentacleStartIndex_ + t * particlesPerTentacle,
	        stiffness,
	        ConstraintType::Vertical
	        );

        for (int p = 0; p < particlesPerTentacle - 1; ++p) {
            addConstraint(
                tentacleStartIndex_ + t * particlesPerTentacle + p,
                tentacleStartIndex_ + t * particlesPerTentacle + p + 1,
                stiffness,
                ConstraintType::Vertical
            );
        }

        for (int p = 0; p + 2 < particlesPerTentacle; ++p) {
            addConstraint(
                tentacleStartIndex_ + t * particlesPerTentacle + p,
                tentacleStartIndex_ + t * particlesPerTentacle + p + 2,
                stiffness * 0.3f,
                ConstraintType::Vertical
            );
        }
    }
}


void ParticleSystem::applyTentacleGravity(float g) {
    for (int i = tentacleStartIndex_; i < static_cast<int>(particles.size()); ++i) {
	    particles[i].acceleration += glm::vec3(0.0f, -g, 0.0f);
    }
}


void ParticleSystem::applyTentacleInwardBias(float strength) {
    const float targetRadius = 0.25f;
    for (int i = tentacleStartIndex_; i < static_cast<int>(particles.size()); ++i) {
        const glm::vec3& pos = particles[i].position;
        glm::vec2 radial(pos.x, pos.z);
        float r = glm::length(radial);
        if (r < 1e-5f) continue;
        float taper = glm::smoothstep(targetRadius, targetRadius * 2.0f, r);
        glm::vec2 inward = -radial / r;
        particles[i].acceleration += glm::vec3(inward.x, 0.0f, inward.y) * strength * taper;
    }
}


void ParticleSystem::applyTentacleBuoyancy(float buoyancy) {
    for (int i = tentacleStartIndex_; i < static_cast<int>(particles.size()); ++i) {
        if (particles[i].pinned) continue;
        particles[i].acceleration += glm::vec3(0.0f, buoyancy, 0.0f);
    }
}


void ParticleSystem::regenerateTentacles(int count, int particlesPerTentacle, float stiffness, int anchorRing) {
    if (tentacleStartIndex_ > 0 &&
        tentacleStartIndex_ < static_cast<int>(particles.size())
    ) particles.erase(particles.begin() + tentacleStartIndex_, particles.end());

    int firstTentacle = tentacleStartIndex_;
    constraints.erase(
        std::remove_if(constraints.begin(), constraints.end(),
            [firstTentacle](const Constraint& c) {
                return static_cast<int>(c.indexA) >= firstTentacle || static_cast<int>(c.indexB) >= firstTentacle;
            }),
            constraints.end()
    );

    tentacleInitialPositions_.clear();
    tentacleRimIndices_.clear();
    innerTentacleRimIndices_.clear();
    innerTentacleStartIndex_ = 0;
    innerTentacleCount_ = 0;
    innerTentacleParticlesPerTentacle_ = 0;
    strandRimIndices_.clear();
    strandStartIndex_ = 0;
    strandCount_ = 0;
    strandParticlesPerTentacle_ = 0;
    generateTentacles(count, particlesPerTentacle, stiffness, anchorRing);
}


void ParticleSystem::generateInnerTentacles(int count, int particlesPerTentacle, float stiffness, int anchorRing) {
    innerTentacleStartIndex_ = static_cast<int>(particles.size());
    innerTentacleCount_ = count;
    innerTentacleParticlesPerTentacle_ = particlesPerTentacle;
    innerTentacleRimIndices_.clear();

    const float spacing = 0.1f;
    int ring = (anchorRing >= 0) ? anchorRing : (rings_ / 3);
    if (ring < 1) ring = 1;
    if (ring > rings_) ring = rings_;
    innerTentacleAnchorRing_ = ring;

    for (int t = 0; t < count; ++t) {
        int slice = static_cast<int>(std::round(static_cast<float>(t) * static_cast<float>(slices_) / static_cast<float>(count))) % slices_;
        int rimIndex = innerTentacleAnchorRing_ * (slices_ + 1) + slice;
        innerTentacleRimIndices_.push_back(rimIndex);
        glm::vec3 rootPos = particles[rimIndex].position;

        float thisSpacing = spacing * tentacleLengthScale(t * 6151 + 101);

        for (int p = 0; p < particlesPerTentacle; ++p) {
            Particle particle{};
            particle.position = rootPos + glm::vec3(0.0f, -static_cast<float>(p + 1) * thisSpacing, 0.0f);
            particle.previousPosition = particle.position;
            particle.acceleration = glm::vec3(0.0f);
            particle.pinned = false;
            tentacleInitialPositions_.push_back(particle.position);
            particles.push_back(particle);
        }

        addConstraint(
            rimIndex,
            innerTentacleStartIndex_ + t * particlesPerTentacle,
            stiffness,
            ConstraintType::Vertical
        );

        for (int p = 0; p < particlesPerTentacle - 1; ++p) {
            addConstraint(
                innerTentacleStartIndex_ + t * particlesPerTentacle + p,
                innerTentacleStartIndex_ + t * particlesPerTentacle + p + 1,
                stiffness,
                ConstraintType::Vertical
            );
        }

        for (int p = 0; p + 2 < particlesPerTentacle; ++p) {
            addConstraint(
                innerTentacleStartIndex_ + t * particlesPerTentacle + p,
                innerTentacleStartIndex_ + t * particlesPerTentacle + p + 2,
                stiffness * 0.3f,
                ConstraintType::Vertical
            );
        }
    }
}


std::vector<glm::vec3> ParticleSystem::getInnerTentaclePositions() const {
    std::vector<glm::vec3> positions;
    positions.reserve(innerTentacleCount_ * (innerTentacleParticlesPerTentacle_ + 1));
    for (int t = 0; t < innerTentacleCount_; ++t) {
        positions.push_back(particles[innerTentacleRimIndices_[t]].position);
        for (int p = 0; p < innerTentacleParticlesPerTentacle_; ++p) {
            positions.push_back(particles[innerTentacleStartIndex_ + t * innerTentacleParticlesPerTentacle_ + p].position);
        }
    }
    return positions;
}


void ParticleSystem::generateStrandTentacles(int count, int particlesPerTentacle, float stiffness, int anchorRing) {
    strandStartIndex_ = static_cast<int>(particles.size());
    strandCount_ = count;
    strandParticlesPerTentacle_ = particlesPerTentacle;
    strandRimIndices_.clear();

    const float spacing = 0.12f;
    int ring = (anchorRing >= 0) ? anchorRing : (rings_ / 6);
    if (ring < 1) ring = 1;
    if (ring > rings_) ring = rings_;
    strandAnchorRing_ = ring;

    for (int t = 0; t < count; ++t) {
        int slice = static_cast<int>(std::round(static_cast<float>(t) * static_cast<float>(slices_) / static_cast<float>(count))) % slices_;
        int rimIndex = strandAnchorRing_ * (slices_ + 1) + slice;
        strandRimIndices_.push_back(rimIndex);
        glm::vec3 rootPos = particles[rimIndex].position;

        float thisSpacing = spacing * (0.7f + 0.6f * (tentacleLengthScale(t * 4051 + 233) - 0.8f) / 0.4f);

        for (int p = 0; p < particlesPerTentacle; ++p) {
            Particle particle{};
            particle.position = rootPos + glm::vec3(0.0f, -static_cast<float>(p + 1) * thisSpacing, 0.0f);
            particle.previousPosition = particle.position;
            particle.acceleration = glm::vec3(0.0f);
            particle.pinned = false;
            tentacleInitialPositions_.push_back(particle.position);
            particles.push_back(particle);
        }

        addConstraint(
            rimIndex,
            strandStartIndex_ + t * particlesPerTentacle,
            stiffness,
            ConstraintType::Vertical
        );

        for (int p = 0; p < particlesPerTentacle - 1; ++p) {
            addConstraint(
                strandStartIndex_ + t * particlesPerTentacle + p,
                strandStartIndex_ + t * particlesPerTentacle + p + 1,
                stiffness,
                ConstraintType::Vertical
            );
        }
    }
}


std::vector<glm::vec3> ParticleSystem::getStrandTentaclePositions() const {
    std::vector<glm::vec3> positions;
    positions.reserve(strandCount_ * (strandParticlesPerTentacle_ + 1));
    for (int t = 0; t < strandCount_; ++t) {
        positions.push_back(particles[strandRimIndices_[t]].position);
        for (int p = 0; p < strandParticlesPerTentacle_; ++p) {
            positions.push_back(particles[strandStartIndex_ + t * strandParticlesPerTentacle_ + p].position);
        }
    }
    return positions;
}


void ParticleSystem::applyApexLift(float strength, float maxLift, const std::vector<glm::vec3>& restPositions) {
    for (size_t i = 0; i < restPositions.size(); ++i) {
        if (particles[i].pinned) continue;
        float yRest = restPositions[i].y;

        float w = glm::smoothstep(0.50f, 0.76f, yRest);
        if (w < 0.0f) continue;

        glm::vec3 target = restPositions[i] + glm::vec3(0.0f, maxLift * w, 0.0f);
        glm::vec3 toTarget = target - particles[i].position;
        float len = glm::length(toTarget);
        if (len < 1e-6f) continue;

        float clampedLen = glm::min(len, maxLift);
        particles[i].acceleration += (toTarget / len) * (strength * clampedLen);
    }
}


std::vector<glm::vec3> ParticleSystem::getTentaclePositions() const {
    std::vector<glm::vec3> positions;
    positions.reserve(tentacleCount_ * (tentacleParticlesPerTentacle_ + 1));
    for (int t = 0; t < tentacleCount_; ++t) {
	    positions.push_back(particles[tentacleRimIndices_[t]].position);
	    for (int p = 0; p < tentacleParticlesPerTentacle_; ++p) {
	        positions.push_back(particles[tentacleStartIndex_ + t * tentacleParticlesPerTentacle_ + p].position);
	    }
    }
    return positions;
}
