//
// Created by hamji on 5/4/2026.
//
#include "imgui.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gui_manager.hpp"

#include <GL/gl.h>

#include "orbit_camera.hpp"

GuiManager::GuiManager(
    PhysicsParams& physics,
    VisualParams& visual,
    CameraParams& camera,
    orbitCamera& orbit_cam
    ) : physics_(physics), visual_(visual), camera_(camera), orbit_cam_(orbit_cam) {}

void GuiManager::render(bool& resetRequested, bool& wireframe) {
    ImGui::Begin("Physics Tuning");
    ImGui::SliderFloat("Repulsor", &physics_.repulsorStrength, 0.0f, 1.0f);
    ImGui::SliderFloat("Frequency", &physics_.contractionFreq, 0.1f, 3.0f);
    ImGui::SliderFloat("Amplitude", &physics_.contractionAmp, 0.0f, 0.5f);
    ImGui::SliderFloat("Damping", &physics_.damping, 0.9f, 1.0f);
    ImGui::SliderInt("Iterations", &physics_.relaxIterations, 1, 16);
    ImGui::SliderFloat("Shape Match", &physics_.shapeMatchingStrength, 0.0f, 1.0f);
    ImGui::SliderFloat("Tentacle Stiffness", &physics_.tentacleStiffness, 0.0f, 1.0f);
    ImGui::SliderFloat("Tentacle G", &physics_.tentacleGravity, 0.0f, 0.2f);
    ImGui::SliderFloat("Tentacle Inwardness", &physics_.tentacleInward, 0.0f, 0.3f);
    ImGui::SliderFloat("Tentacle Damping", &physics_.tentacleDamping, 0.85f, 1.0f);
    ImGui::SliderFloat("Tentacle Buoyancy", &physics_.tentacleBuoyancy, 0.0f, 0.05f);
    ImGui::SliderFloat("Apex Lift", &physics_.apexLift,0.0f, 2.0f);
    ImGui::SliderFloat("Apex Max Lift", &physics_.apexMaxLift,0.0f, 0.15f);
    ImGui::SliderFloat("Fade-In", &physics_.fadeInDuration, 0.5f, 5.0f);
    ImGui::SliderFloat("Power Stroke", &physics_.powerStrokeFraction, 0.1f, 0.6f);
    resetRequested = ImGui::Button("Reset Simulation");
    ImGui::End();

    ImGui::Begin("Visual");
    ImGui::SliderFloat("Fresnel", &visual_.fresnelPower, 0.5f, 8.0f);
    ImGui::SliderFloat("Absorption", &visual_.absorptionCoeff, 0.0f, 5.0f);
    ImGui::SliderFloat("Ambient", &visual_.ambientStrength, 0.0f, 1.0f);
    ImGui::ColorEdit3("Body Color", glm::value_ptr(visual_.bodyColor));
    ImGui::DragFloat3("Light Pos", glm::value_ptr(visual_.lightPos), 0.1, -10.0f, 10.0f);
    ImGui::ColorEdit3("Light Color", glm::value_ptr(visual_.lightColor));
    ImGui::SliderFloat("Tube Radius", &visual_.tubeRadius, 0.005f, 0.06f);
    ImGui::SliderFloat("Tube Tip", &visual_.tubeRadiusTip, 0.05f, 1.0f);
    ImGui::SliderFloat("Iridescence", &visual_.iridescence, 0.0f, 1.0f);
    ImGui::SliderFloat("Bio Strength", &visual_.bioStrength, 0.0f, 2.0f);
    ImGui::ColorEdit3("Bio Color", glm::value_ptr(visual_.bioColor));
    ImGui::ColorEdit3("Bg Top", glm::value_ptr(visual_.bgTopColor));
    ImGui::ColorEdit3("Bg Bottom", glm::value_ptr(visual_.bgBottomColor));
    ImGui::Checkbox("Wireframe", &wireframe);
    ImGui::End();

    ImGui::Begin("Camera");
    float azDeg = glm::degrees(camera_.azimuth);
    float plDeg = glm::degrees(camera_.polar);
    bool changed = false;
    changed |= ImGui::SliderFloat("Azimuth", &azDeg, 0.0f, 360.0f);
    changed |= ImGui::SliderFloat("Polar", &plDeg, -80.0f, 80.0f);
    changed |= ImGui::SliderFloat("Radius", &camera_.radius, 1.0f, 20.0f);
    changed |= ImGui::SliderFloat("FOV", &camera_.fov, 20.0f, 90.0f);
    if (changed) {
        camera_.azimuth = glm::radians(azDeg);
        camera_.polar = glm::radians(plDeg);
        orbit_cam_.setAzimuth(camera_.azimuth);
        orbit_cam_.setPolar(camera_.polar);
        orbit_cam_.setRadius(camera_.radius);
    }
    ImGui::End();

    ImGui::Begin("Status");
    ImGui::Text("%.1f fps", ImGui::GetIO().Framerate);
    ImGui::End();
}