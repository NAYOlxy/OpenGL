#pragma once
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


class Camera {
public:

	Camera();
	~Camera();

public:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;

	bool firstMouse;
	float yaw;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	float pitch;
	float lastX;
	float lastY;
	float fov;

	float deltaTime;	// time between current frame and last frame
	float lastFrame;
};