#include "SpectatorCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

SpectatorCamera::SpectatorCamera(
    glm::vec3 position, float yaw, float pitch, float fov, float aspect, float nearPlane, float farPlane)
    : position(position), yaw(yaw), pitch(pitch), fov(fov), aspect(aspect), nearPlane(nearPlane), farPlane(farPlane),
      movementSpeed(20.0f), mouseSensitivity(0.1f), worldUp(0.0f, 1.0f, 0.0f)
{
    updateCameraVectors();
}

void SpectatorCamera::processKeyboard(float deltaTime, bool forward, bool backward, bool left, bool right, bool up, bool down, float speedMultiplier) {
    float velocity = movementSpeed * deltaTime * speedMultiplier;
    if (forward)  position += front * velocity;
    if (backward) position -= front * velocity;
    if (left)     position -= right * velocity;
    if (right)    position += right * velocity;
    if (up)       position += worldUp * velocity;
    if (down)     position -= worldUp * velocity;
}

void SpectatorCamera::processMouse(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if (constrainPitch) {
        if (pitch > 89.0f)  pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }
    updateCameraVectors();
}

void SpectatorCamera::updateAspect(float newAspect) {
    aspect = newAspect;
}

glm::mat4 SpectatorCamera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 SpectatorCamera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

void SpectatorCamera::updateCameraVectors() {
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(f);
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}
