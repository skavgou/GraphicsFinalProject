#include "Camera.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

// Constructor: Initialize the camera with default values
Camera::Camera(glm::vec3 startPosition)
    : position(startPosition), lookAt(glm::vec3(0.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)),
      azimuth(0.0f), polar(0.0f), distance(500.0f) {}

// Generates the view matrix
glm::mat4 Camera::getViewMatrix() const {
    glm::vec3 eyePosition = glm::vec3(
        distance * cos(polar) * cos(azimuth),
        distance * sin(polar),
        distance * cos(polar) * sin(azimuth)
    ) + position;

    std::cout << "Eye Position: " << eyePosition.x << ", " << eyePosition.y << ", " << eyePosition.z << std::endl;
    std::cout << "LookAt: " << lookAt.x << ", " << lookAt.y << ", " << lookAt.z << std::endl;

    return glm::lookAt(eyePosition, lookAt, up);
}




// Returns the camera's position
const glm::vec3& Camera::getPosition() const {
    return position;
}

void Camera::handleKeyInput(int key, int action) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    constexpr float moveStep = 10.0f;

    switch (key) {
        case GLFW_KEY_W: // Move forward
            position.z -= moveStep;
        break;
        case GLFW_KEY_S: // Move backward
            position.z += moveStep;
        break;
        case GLFW_KEY_A: // Move left
            position.x -= moveStep;
        break;
        case GLFW_KEY_D: // Move right
            position.x += moveStep;
        break;
    }

    std::cout << "Updated Camera Position: " << position.x << ", " << position.y << ", " << position.z << std::endl;
}




// Resets the camera to its default state
void Camera::reset() {
    azimuth = 0.0f;
    polar = 0.0f;
    distance = 500.0f;
    position = glm::vec3(0.0f, 50.0f, 0.0f);
    std::cout << "Camera reset." << std::endl;
}
