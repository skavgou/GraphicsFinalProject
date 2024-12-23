#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
	glm::vec3 position;  // Camera's current position
	glm::vec3 lookAt;    // Target the camera is looking at
	glm::vec3 up;        // Up direction for the camera

	float azimuth;       // Horizontal angle
	float polar;         // Vertical angle
	float distance;      // Distance from the lookAt target

public:
	Camera(glm::vec3 startPosition = glm::vec3(0.0f, 50.0f, 0.0f));

	glm::mat4 getViewMatrix() const;

	// Getters
	const glm::vec3& getPosition() const;

	// Input handling
	void handleKeyInput(int key, int action);

	// Reset camera
	void reset();
};

#endif // CAMERA_H
