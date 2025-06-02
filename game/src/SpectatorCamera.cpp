#include "SpectatorCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iostream> // Added iostream
#include <ostream> // Added ostream
#include "utils/debug_logger.h"

SpectatorCamera::SpectatorCamera(
    glm::vec3 position, float yaw, float pitch, float fov, float aspect, float nearPlane, float farPlane)
    : position(position), yaw(yaw), pitch(pitch), fov(fov), aspect(aspect), nearPlane(nearPlane), farPlane(farPlane),
      movementSpeed(20.0f), mouseSensitivity(0.1f), worldUp(0.0f, 1.0f, 0.0f)
{
    updateCameraVectors();
}

void SpectatorCamera::processKeyboard(float deltaTime, bool forward, bool backward, bool left, bool right, bool up, bool down, float speedMultiplier) {
    float velocity = movementSpeed * deltaTime * speedMultiplier;
    // Always use a horizontal right vector for left/right movement
    glm::vec3 horizontalFront = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    glm::vec3 horizontalRight = glm::normalize(glm::cross(horizontalFront, worldUp));
    if (forward)  position += horizontalFront * velocity;
    if (backward) position -= horizontalFront * velocity;
    if (left)     position -= horizontalRight * velocity;
    if (right)    position += horizontalRight * velocity;
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
    // Only log if there's a meaningful change
    if (std::abs(aspect - newAspect) > 0.001f) {
        DEBUG_LOG("SpectatorCamera", "Aspect ratio updated: " + std::to_string(aspect) + " -> " + std::to_string(newAspect));
    }
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
