//
// Created by hamji on 5/6/2026.
//

#include "particle_system.hpp"

#include <cmath>
#include <set>
#include <map>


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


void ParticleSystem::integrate(float dt, float damping) {
    for (auto& p : particles) {
        if (p.pinned) continue;

        glm::vec3 velocity = p.position - p.previousPosition;
        p.previousPosition = p.position;

        p.position = p.position + (velocity * damping) + (p.acceleration * dt * dt);

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


void ParticleSystem::applyContraction(float time, float frequency, float amplitude, const std::vector<glm::vec3>& restPositions) {
    float wave = glm::sin(time * frequency * 2.0f * glm::pi<float>());
    glm::vec3 center = glm::vec3(0.0f, 0.2f, 0.0f);  // center of bell interior

    for (size_t i = 0; i < restPositions.size(); ++i) {
        if (particles[i].pinned) continue;

        // Use Rest position for stable direction
        glm::vec3 toCenter = center - restPositions[i];
        float dist = glm::length(toCenter);
        if (dist < 1e-6f) continue;

        float heightFactor = 1.0f - glm::clamp(restPositions[i].y / 0.7f, 0.0f, 1.0f);
        float strength = amplitude * wave * heightFactor;
        particles[i].acceleration += (toCenter / dist) * strength;
    }
}


void ParticleSystem::applyRepulsor(glm::vec3 center, float strength) {
    for (auto& p : particles) {
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


void ParticleSystem::generateTentacles(int count, int particlesPerTentacle, float stiffness) {
    tentacleStartIndex_ = static_cast<int>(particles.size());
    tentacleCount_ = count;
    tentacleParticlesPerTentacle_ = particlesPerTentacle;
    tentacleRimIndices_.clear();

    const float spacing = 0.1f;

    for (int t = 0; t < count; ++t) {
	    int slice = static_cast<int>(std::round(static_cast<float>(t) * static_cast<float>(slices_) / static_cast<float>(count))) % slices_;
	    int rimIndex = rings_ * (slices_ + 1) + slice;
	    tentacleRimIndices_.push_back(rimIndex);
	    glm::vec3 rootPos = particles[rimIndex].position;

	    for (int p = 0; p < particlesPerTentacle; ++p) {
	        Particle particle{};
	        particle.position = rootPos + glm::vec3(0.0f, -static_cast<float>(p + 1) * spacing, 0.0f);
	        particle.previousPosition = particle.position;
	        particle.acceleration = glm::vec3(0.0f);
	        particle.pinned = false;
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
    }
}


void ParticleSystem::applyTentacleGravity(float g) {
    for (int i = tentacleStartIndex_; i < static_cast<int>(particles.size()); ++i) {
	    particles[i].acceleration += glm::vec3(0.0f, -g, 0.0f);
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
