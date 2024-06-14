#include "Camera.h"

Camera::Camera()
	:cameraPos(glm::vec3(0.0f, 0.0f, 3.0f)),
	cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)),
	cameraUp (glm::vec3(0.0f, 1.0f, 0.0f)),
	firstMouse(true),yaw(-90.0f),pitch(0.0f),
	lastX(960.0f / 2.0),lastY(540.0f / 2.0),
	fov(45.0f),deltaTime(0.0f),lastFrame(0.0f)
{
}

Camera::~Camera()
{
}
