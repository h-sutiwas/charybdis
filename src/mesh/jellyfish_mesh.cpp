//
// Created by hamji on 5/5/2026.
//
#include "jellyfish_mesh.hpp"

#include <cmath>
#include <glm/glm.hpp>



Mesh generateDomeMesh(int rings, int slices, float radiusX, float radiusY) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const float perturbAmp = 0.015f;
    for (int ring = 0; ring <= rings; ++ring) {
        float phi = (glm::pi<float>() / 2) * ring / rings;

        for (int slice = 0; slice <= slices; ++slice) {
            float theta = (glm::pi<float>() * 2) * slice / slices;

            // Position on ellipsoid surface
            float x = radiusX * glm::sin(phi) * glm::cos(theta);
            float y = radiusY * glm::cos(phi);
            float z = radiusX * glm::sin(phi) * glm::sin(theta);

            // Normal for ellipsoid
            float nx = x / (radiusX * radiusX);
            float ny = y / (radiusY * radiusY);
            float nz = z / (radiusX * radiusX);
            glm::vec3 normal = glm::normalize(glm::vec3(nx, ny, nz));

            float perturbR = glm::sin(3.0f * theta) * glm::sin(2.0f * phi) * perturbAmp;
            float perturbY = glm::sin(2.0f * theta + 1.7f) * glm::sin(2.0f * phi) * 0.5f * perturbAmp;
            x += perturbR * glm::cos(theta);
            z += perturbR * glm::sin(theta);
            y += perturbY;

            vertices.push_back(Vertex{glm::vec3(x, y, z), normal });
        }
    }

    for (int ring = 0; ring <= rings - 1; ++ring) {
        for (int slice = 0; slice <= slices - 1; ++slice) {
            // 4 corners
            unsigned int topLeft = ring * (slices + 1) + slice;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (ring + 1) * (slices + 1) + slice;
            unsigned int bottomRight = bottomLeft + 1;

            // Triangle 1
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Triangle 2
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    return Mesh(vertices, indices);
};