#ifndef SPECTATOR_CAMERA_H
#define SPECTATOR_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class SpectatorCamera {
public:
    SpectatorCamera(
        glm::vec3 position = glm::vec3(16.0f, 24.0f, 48.0f),
        float yaw = -90.0f, float pitch = 0.0f,
        float fov = 70.0f, float aspect = 4.0f/3.0f,
        float nearPlane = 0.1f, float farPlane = 500.0f
    );

    void processKeyboard(float deltaTime, bool forward, bool backward, bool left, bool right, bool up, bool down, float speedMultiplier = 1.0f);
    void processMouse(float xoffset, float yoffset, bool constrainPitch = true);
    void updateAspect(float aspect);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getFront() const { return front; }
    glm::vec3 getUp() const { return up; }
    glm::vec3 getRight() const { return right; }
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }

    void setPosition(const glm::vec3& pos) { position = pos; }
    void setYaw(float y) { yaw = y; updateCameraVectors(); }
    void setPitch(float p) { pitch = p; updateCameraVectors(); }

    float movementSpeed = 20.0f;
    float mouseSensitivity = 0.1f;
    float fov;
    float aspect;
    float nearPlane;
    float farPlane;

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    float yaw;
    float pitch;

    void updateCameraVectors();
};

#endif // SPECTATOR_CAMERA_H
